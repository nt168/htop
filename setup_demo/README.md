# Setup Demo

This is a minimal, standalone extraction of htop's `Panel`/`ScreenManager`-style UI
pattern. It compiles into a small TUI that reproduces the Setup menu behavior:
left categories, middle items, and right-side details that update as you move
through the lists.

## Build

```sh
make
```

## Run

```sh
./setup_demo
```

Keys:
- `Tab`: switch focus between columns
- `↑/↓`: move selection
- `q`: quit
