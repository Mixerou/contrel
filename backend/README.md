# Contrel Backend

Backend module of the Contrel Project

## General Commands

```bash
# Develop with bacon
$ cargo install --locked bacon
$ bacon

# Build for production (Only Tier 1 targets recommended)
$ cargo build --release --target=<arch><sub>-<vendor>-<sys>-<abi>
```

## Formatting

We are using unstable Rustfmt options,
so you need to run the `cargo fmt` command with the nightly toolchain.

```bash
# Add a nightly toolchain
$ rustup toolchain add nightly

# Format the code
$ cargo +nightly fmt
```

## Environment Variables

| Variable           | Default Value | Description                                                                                                                   |
|--------------------|:-------------:|-------------------------------------------------------------------------------------------------------------------------------|
| `RUST_LOG`         |       -       | `env_logger` output controller. Module declarations take comma separated entries formatted like `path::to::module=log_level`. |
| `BACKEND_IP`       |  `127.0.0.1`  | IP on which the Backend will run.                                                                                             |
| `BACKEND_PORT`     |    `5000`     | The port that the Backend will listen to.                                                                                     |
| `BACKEND_API_PATH` | Empty string  | Domain path to the API. Do not add `/` at the end.                                                                            |
