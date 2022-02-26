use std::io;
use std::io::Write;

mod save_manager {
    extern crate fs_extra;
    extern crate dirs;

    use fs_extra::dir;
    use std::{fs, path, env, io, error};
    
    pub fn get_saves_folder() -> Result<String, Box<dyn error::Error>> {
        Ok(env::current_exe()?
            .parent()
            .ok_or(io::Error::new(io::ErrorKind::InvalidData, "Current executable does not have a directory"))?
            .to_path_buf()
            .clone()
            .into_os_string()
            .into_string()
            .map_err(|_| io::Error::new(io::ErrorKind::InvalidData, "Failed convert path to string"))?
            + "\\saves")
    }
    
    pub fn get_noita_save_folder() -> Result<String, Box<dyn error::Error>> {
        match env::consts::OS {
            "linux" => Ok(String::from("~/.steam/steam")),
            "windows" => Ok(
                dirs::home_dir()
                    .ok_or(io::Error::new(io::ErrorKind::InvalidData, "Failed to get user profile"))?
                    .into_os_string()
                    .into_string()
                    .map_err(|_|
                        Box::new(io::Error::new(io::ErrorKind::InvalidData, "Failed to convert path to string")))?
                    + "/AppData/LocalLow/Nolla_Games_Noita"
            ),
            _ => Err(Box::new(io::Error::new(io::ErrorKind::InvalidData, "Unsupported OS"))),
        }
    }

    pub fn save(save_name: &str) -> Result<(), Box<dyn error::Error>> {
        let save_path = get_saves_folder()? + "\\" + save_name;

        fs::create_dir_all(&save_path)?;

        let mut options = dir::CopyOptions::new();
        options.overwrite = true;
        options.copy_inside = true;

        dir::copy(
            get_noita_save_folder()? + "\\save00",
            save_path,
            &options
        )?;

        Ok(())
    }

    pub fn load(save_name: &str) -> Result<(), Box<dyn error::Error>> {
        let save_path = match get_save_path(save_name) {
            Some(path) => path,
            None => return Err(Box::new(io::Error::new(io::ErrorKind::InvalidInput, "Failed find save"))),
        };
        
        let mut options = dir::CopyOptions::new();
        options.overwrite = true;
        options.copy_inside = true;

        fs::create_dir_all(get_noita_save_folder()?)?;
        
        dir::copy(
            save_path.to_str()
                .ok_or(io::Error::new(io::ErrorKind::InvalidInput, "Failed to convert path to string"))?
                .to_string()
            + "\\save00",
            get_noita_save_folder()?,
            &options
        )?;
        
        Ok(())
    }
    
    pub fn remove(save_name: &str) -> Result<bool, Box<dyn error::Error>> {
        let save_path = match get_save_path(save_name) {
            Some(path) => path,
            None => return Err(Box::new(io::Error::new(io::ErrorKind::InvalidInput, "Failed to find save"))),
        };
        
        println!("About to delete {}, are you sure? [y/n]", save_name);
        let mut input = String::new();
        match io::stdin().read_line(&mut input) {
            Err(_) => return Err(Box::new(io::Error::new(io::ErrorKind::InvalidInput, "Failed to get response"))),
            _ => (),
        };
        
        if input.trim() == "y" || input.trim() == "Y" {
            fs::remove_dir_all(save_path)?;
            Ok(true)
        } else {
            Ok(false)
        }
    }
    
    pub fn list() -> Result<(), Box<dyn error::Error>> {
        for entry in fs::read_dir(get_saves_folder()?)? {
            println!("{}", entry?
                .file_name()
                .to_str()
                .ok_or(io::Error::new(io::ErrorKind::InvalidData, "Failed print save"))?
            );
        }
        
        Ok(())
    }
    
    fn get_save_path(save_name: &str) -> Option<path::PathBuf> {
        let str = get_saves_folder().ok()? + "\\" + save_name;
        let path = path::Path::new(str.as_str());
        match path.exists() {
            false => None,
            true => Some(path::PathBuf::from(path)),
        }
    }
}

fn main() {
    loop {
        print!("> ");
        match io::stdout().flush() {
            Err(why) => println!("{}", why),
            _ => ()
        };
        
        let mut input = String::new();
        match io::stdin().read_line(&mut input) {
            Err(why) => println!("{}", why),
            _ => ()
        };
        
        match match input.trim().split(' ').collect::<Vec<_>>()[..] {
            ["save", save_name, ..] => {
                let result = save_manager::save(save_name);
                println!("Saved to {}", save_name);
                result
            },
            ["load", save_name, ..] => {
                let result = save_manager::load(save_name);
                println!("Loaded {}", save_name);
                result
            },
            ["remove", save_name, ..] => {
                let result = save_manager::remove(save_name);
                match result {
                    Ok(true) => {
                        println!("Removed {}", save_name);
                        Ok(())
                    },
                    Ok(false) => {
                        println!("Cancelled removal of {}", save_name);
                        Ok(())
                    },
                    _ => Err(result.err().unwrap()),
                }
            },
            ["list", ..] => save_manager::list(),
            ["quit", ..] => return,
            _ => Ok(println!("Command does not exist")),
        } {
            Err(why) => println!("{}", why),
            _ => ()
        }
    }
}
