# Puyo Puyo Tsu AI
An AI designed to play Puyo Puyo Tsu 1P and PVP. This project presently only compiles with `gcc` and runs on window.

## Build
- Clone and `cd` to the repository.
- Run `make` to build the client.
- Run `make USE_PEXT=true` for best performance, check if your cpu supports `pext`.
- Get the binary in `bin` folder.
- For now only `gcc` and window.

## Directory description
- `core` - puyo puyo core logic implementation.
- `ai` - the AI implementation.
- `cli` - the standalone client, for now only showing the AI playing 1P.

## License
This project is licensed under [MIT LICENSE](LICENSE).
