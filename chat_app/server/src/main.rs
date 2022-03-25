use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use tokio::{
    io::{ AsyncBufReadExt, BufReader },
    net::TcpListener,
    sync::broadcast,
};
use tokio::io::AsyncWriteExt;

#[tokio::main]
async fn main() {
    let listener = TcpListener::bind("localhost:6767").await.unwrap();
    println!("Server started");
    
    let main_clients = Arc::new(Mutex::new(HashMap::new()));
    
    let (tx, _rx) = broadcast::channel::<String>(16);
    
    loop {
        let (mut socket, addr) = listener.accept().await.unwrap();

        let clients = Arc::clone(&main_clients);
        let tx = tx.clone();
        let mut rx = tx.subscribe();
        
        tokio::spawn(async move {
            let (reader, mut writer) = socket.split();
            let mut reader = BufReader::new(reader);
            let mut msg = String::new();

            let mut name = String::new();
            reader.read_line(&mut name).await.unwrap();
            name = name.trim().to_owned();
            
            clients.lock().unwrap().insert(addr, name.clone());
            println!("{} ({}) connected", name, addr);
            
            loop {
                tokio::select! {
                    result = reader.read_line(&mut msg) => {
                        let byte_read = match result {
                            Ok(byte_read) => byte_read,
                            Err(_) => break,
                        };
        
                        msg = msg.trim_end().to_owned();
        
                        if byte_read == 0 {
                            break;
                        }
        
                        println!("{} ({}): {}", name, addr, msg);
                        
                        tx.send(format!("{}: {}", name, msg)).unwrap();
                    },
                    result = rx.recv() => {
                        writer.write_all(format!("{}\n", result.unwrap()).as_bytes()).await.unwrap();
                    },
                }
            }

            println!("{} ({}) disconnected", name, addr);
            clients.lock().unwrap().remove(&addr);
        });
    }
}
