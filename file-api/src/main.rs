use libc::{O_CREAT, O_RDONLY, O_TRUNC, O_WRONLY};
use libc::{S_IRGRP, S_IROTH, S_IRUSR, S_IWGRP, S_IWOTH, S_IWUSR};
use libc::{close, open, read, write};
use std::ffi::{CString, OsString, c_uint};
use std::io;
use std::path::PathBuf;

fn main() {
    let input_path = first_arg();

    let input_fd = unsafe { open(input_path.as_ptr(), O_RDONLY) };
    if input_fd == -1 {
        panic!("error opening output file: {}", errno());
    }

    let output_path = random_filename();
    let open_flags = O_CREAT | O_WRONLY | O_TRUNC;
    let mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH; // rw-rw-rw-

    let output_fd = unsafe { open(output_path.as_ptr(), open_flags, mode as c_uint) };
    if output_fd == -1 {
        panic!("error opening output file: {}", errno());
    }
    println!("created file: {output_path:?}");

    const BUF_SIZE: usize = 1024;
    let mut buf = [0_u8; BUF_SIZE];

    loop {
        let num_read = unsafe { read(input_fd, buf.as_mut_ptr().cast(), BUF_SIZE) };

        match num_read {
            0 => break,

            -1 => panic!("error reading input file: {}", errno()),

            _ => {
                let num_read = assert_non_negative(num_read);
                let num_write = unsafe { write(output_fd, buf.as_ptr().cast(), num_read) };

                if num_write == -1 {
                    panic!("error writing to output file: {}", errno());
                } else {
                    let num_write = assert_non_negative(num_write);

                    if num_write != num_read {
                        assert!(num_write < num_read);
                        panic!("couldn't write whole buffer");
                    }
                }
            }
        }
    }

    for fd in [input_fd, output_fd] {
        if unsafe { close(fd) } == -1 {
            panic!("error when closing file: {}", errno())
        }
    }
}

fn first_arg() -> CString {
    let mut args = std::env::args_os();

    let program = args.next().unwrap();

    if let Some(arg) = args.next()
        && args.next().is_none()
    {
        let bytes = arg.into_encoded_bytes();
        CString::new(bytes).expect("valid CString")
    } else {
        let program = PathBuf::from(program)
            .file_name()
            .map(OsString::from)
            .unwrap_or_else(|| OsString::from("program"));
        let program = program.display();

        eprintln!("usage: {program} <file>");
        std::process::exit(1);
    }
}

// why are these `unsafe`? ("unchecked")
// rustc cannot verify the conditions the operating system requires
//
// `open`: path ptr must be valid C string (null-terminated)
// `read` and `write`: buffer ptr must be valid, point to valid memory of claimed size
// `close`: fd may be invalid or already closed (double-close races)

fn errno() -> io::Error {
    io::Error::last_os_error()
}

fn assert_non_negative(signed: isize) -> usize {
    usize::try_from(signed).expect("if negative, must be -1")
}

fn random_filename() -> CString {
    use rand::distr::{Alphanumeric, Distribution};

    let bytes: Vec<u8> = Alphanumeric.sample_iter(rand::rng()).take(8).collect();
    CString::new(bytes).unwrap()
}
