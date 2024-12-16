# Contrel Desktop Client

Desktop Client module of the Contrel Project

## Dependencies

### External

> All libraries added with `git submodule` except `Glad`

[FreeType]: https://github.com/freetype/freetype

[Glad]: https://github.com/Dav1dde/glad

[Glad Configurator]: https://gen.glad.sh/

[GLFW]: https://github.com/glfw/glfw

[Dear ImGui]: https://github.com/ocornut/imgui

[IXWebSocket]: https://github.com/machinezone/IXWebSocket

[LunaSVG]: https://github.com/sammycage/lunasvg

[mbedtls]: https://github.com/Mbed-TLS/mbedtls

[msgpack]: https://github.com/msgpack/msgpack-c

[stb]: https://github.com/nothings/stb

- [FreeType] — VER-2-13-3
- [Glad] — 2.0.8 (generated from [Glad Configurator])
- [GLFW] — 3.4
- [Dear ImGui] — v1.91.6
- [IXWebSocket] — v11.4.5
- [LunaSVG] — v3.0.1
- [mbedtls] — mbedtls-2.28.9
- [msgpack] — cpp-7.0.0
- [stb] — 0dd01c2

### System

> It is required that these dependencies are already in the system

- OpenGL

### For Development

- just
- cmake
- ninja

## Just Commands

These commands require `just` to be installed.

> Some commands are not optimised for Windows

```bash
# Configure project
$ just configure-debug # For debug (alias: configure)
$ just configure-release # For release

# Build client
$ just build-debug # For debug (alias: build)
$ just build-release # For release

# Run application
$ just run-debug # For debug (alias: run)
$ just run-release # For release

# Clean output directory
$ just clean
```

> There is a chain: project configuration >> build >> run
>
> This means that you can type `just run`,
> and that command will execute `configure` and `build` recipes for you.
>
> Same with `just build` command,
> which will run `configure` script before executing.
