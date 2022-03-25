use crossterm::event::{Event, KeyCode};
use crossterm::terminal;
use crossterm::{event, execute};
use std::collections::LinkedList;
use std::io::Write;
use std::sync::atomic::{AtomicBool, AtomicUsize, Ordering};
use std::sync::{Arc, Mutex};
use std::{error, io};
use tokio::{
    io::{AsyncBufReadExt, AsyncWriteExt, BufReader},
    join,
    net::TcpStream,
};
use tui::layout::{Constraint, Direction, Layout};
use tui::widgets::{Block, Borders, BorderType, Paragraph};
use tui::{backend, Terminal};
use unicode_width::UnicodeWidthStr;

fn input(prompt: &str) -> Result<String, Box<dyn error::Error>> {
    io::stdout().write_all(prompt.as_bytes())?;
    io::Write::flush(&mut io::stdout())?;

    let mut input = String::new();
    io::stdin().read_line(&mut input)?;
    Ok(input)
}

#[tokio::main]
async fn main() {
    let host_addr = input("Enter host address (ip:port): ")
        .unwrap()
        .trim()
        .to_owned();
    let name = input("Enter your name: ").unwrap();

    let mut conn = TcpStream::connect(host_addr).await.unwrap();
    println!("Host connected");
    io::Write::flush(&mut io::stdout()).unwrap();

    conn.write_all(name.as_bytes()).await.unwrap();

    let buf = Arc::new(Mutex::new(String::new()));

    let (reader, mut writer) = conn.into_split();

    let msg_list = Arc::new(Mutex::new(LinkedList::<String>::new()));
    let running = Arc::new(AtomicBool::new(true));
    let view_height = Arc::new(AtomicUsize::new(0));

    let draw_join;
    let conn_join;

    {
        let msg_list = Arc::clone(&msg_list);
        let running = Arc::clone(&running);
        let buf = Arc::clone(&buf);
        let view_height = Arc::clone(&view_height);
        draw_join = tokio::spawn(async move {
            terminal::enable_raw_mode().unwrap();
            let mut stdout = io::stdout();
            execute!(
                stdout,
                terminal::EnterAlternateScreen,
                event::EnableMouseCapture
            )
            .unwrap();
            let backend = backend::CrosstermBackend::new(stdout);
            let mut terminal = Terminal::new(backend).unwrap();

            while running.load(Ordering::Relaxed) {
                terminal
                    .draw(|f| {
                        let chunks = Layout::default()
                            .direction(Direction::Vertical)
                            .margin(1)
                            .constraints([Constraint::Max(3), Constraint::Max(3)].as_ref())
                            .split(f.size());

                        let mut view_cap = 0;
                        if chunks[0].height as usize > 2 {
                            view_cap = chunks[0].height as usize - 2;
                        }

                        if view_cap + view_height.load(Ordering::Relaxed)
                            > msg_list.lock().unwrap().len()
                        {
                            view_height.store(
                                if msg_list.lock().unwrap().len() > view_cap {
                                    msg_list.lock().unwrap().len() - view_cap
                                } else {
                                    0usize
                                },
                                Ordering::Relaxed,
                            );
                        }

                        let msg_view = Paragraph::new(
                            msg_list
                                .lock()
                                .unwrap()
                                .iter()
                                .rev()
                                .take(view_cap + view_height.load(Ordering::Relaxed))
                                .rev()
                                .take(view_cap)
                                .fold(String::new(), |a, b| a + b),
                        )
                        .block(Block::default()
                            .borders(Borders::ALL)
                            .title("Message View"));

                        f.render_widget(msg_view, chunks[0]);

                        let input = Paragraph::new(buf.lock().unwrap().to_string())
                            .block(Block::default().borders(Borders::ALL));

                        f.render_widget(input, chunks[1]);

                        f.set_cursor(
                            chunks[1].x + buf.lock().unwrap().to_string().width() as u16 + 1,
                            chunks[1].y + 1,
                        )
                    })
                    .unwrap();
            }

            terminal::disable_raw_mode().unwrap();
            execute!(
                terminal.backend_mut(),
                terminal::LeaveAlternateScreen,
                event::DisableMouseCapture
            )
            .unwrap();
        });
    }

    {
        let msg_list = Arc::clone(&msg_list);
        let running = Arc::clone(&running);
        conn_join = tokio::spawn(async move {
            let mut msg = String::new();
            let mut reader = BufReader::new(reader);
            while running.load(Ordering::Relaxed) {
                reader.read_line(&mut msg).await.unwrap();
                msg_list.lock().unwrap().push_back(msg.clone());

                while msg_list.lock().unwrap().len() > 500 {
                    msg_list.lock().unwrap().pop_front();
                }

                msg.clear();
            }
        });
    }

    loop {
        if let Event::Key(key) = event::read().unwrap() {
            match key.code {
                KeyCode::Enter => {
                    if buf.lock().unwrap().trim() == ":q" {
                        break;
                    }
                    writer
                        .write_all((format!("{}{}", buf.lock().unwrap(), "\n")).as_bytes())
                        .await
                        .unwrap();
                    buf.lock().unwrap().clear();
                }
                KeyCode::Char(c) => {
                    buf.lock().unwrap().push(c);
                }
                KeyCode::Backspace => {
                    buf.lock().unwrap().pop();
                }
                KeyCode::Up => {
                    view_height.fetch_add(1, Ordering::Relaxed);
                }
                KeyCode::Down => {
                    if view_height.load(Ordering::Relaxed) > 0 {
                        view_height.fetch_sub(1, Ordering::Relaxed);
                    }
                }
                _ => (),
            }
        }
    }

    running.store(false, Ordering::Relaxed);

    let (draw_result, conn_result) = join!(draw_join, conn_join);
    draw_result.unwrap();
    conn_result.unwrap();
}
