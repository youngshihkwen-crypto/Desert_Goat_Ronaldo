# Desert Goat: SIUUUUU

[中文](README_CN.md)

A pixel-art maze shooter written in C++. It runs in a web browser through WebAssembly and also has a native Windows build.

You play as Ronaldo and travel through a procedurally generated maze while rivals and bosses try to stop you. There are nine stages. Each stage uses a different maze seed, and the trophy distance and enemy pressure increase as the game progresses.

## Running the game

Browser version:

```powershell
node serve.cjs
```

Then open <http://127.0.0.1:4173/>.

For the Windows version, run `build/DesertGoat.exe`.

## Controls

| Key | Action |
| --- | --- |
| WASD / Arrow keys | Move; change direction while shielding |
| Space | Shoot a football |
| Hold C | Raise the shield and block attacks from the front |
| V | Use the selected special move |
| B | Use the Morgan Interview once per stage |
| P | Pause |
| M | Mute |
| Enter | Confirm an option or skip a cinematic |

Menus support both keyboard and mouse input.

## How to play

Ronaldo starts near the center of the maze. A thin red line points toward the trophy, but it does not reveal the correct path. The trophy may appear close even when a wall forces the player to take a longer route.

In the first three stages, reaching the trophy completes the stage. From stage four onward, a boss locks the trophy. The boss must be defeated before the trophy can be collected. The stage ends with Ronaldo lifting the trophy and performing the SIUUUUU celebration.

Regular rivals use IDA* pathfinding to chase the player and shoot when they have a clear line of sight. When the `REINFORCEMENTS` meter is full, a new group of rivals appears and Ronaldo recovers one health point. The bar at the top of the screen shows the direct-distance progress toward the trophy.

Before each stage, one of four special moves can be selected:

| Special move | Effect |
| --- | --- |
| Armband Slam | A short-range shockwave that damages, stuns and pushes rivals |
| Ronado | A spinning dash that damages enemies along its path |
| Phone Smash | Throws a phone and creates an area attack at the landing point |
| Elbow Strike | Deals heavy damage directly in front of Ronaldo |

All four special moves can damage bosses. Their energy recharges over time, and normal football hits restore a small amount of energy.

The Morgan Interview is unlocked after clearing the first three stages. It removes visible regular rivals and enemy shots, restores full health, and seals the boss for two seconds. It does not directly damage the boss.

Boss special attacks pause the game and display a WARNING window. After the warning closes, the player still has a short time to move away. Some ground attacks cannot be blocked by the shield.

Messi in stage eight and Maguire in stage nine enter rage mode at half health. A red warning window flashes three times, the boss turns red, and its movement, shooting and special attacks become faster.

## Building

The build requires Clang with WebAssembly support, MinGW g++, and Node.js.

Build the browser version and run the tests:

```powershell
./build.ps1
```

Build and test both the browser and Windows versions:

```powershell
./build.ps1 -Native
```

The root `game.js` file is the generated browser bundle with the WebAssembly binary embedded in it. Running `build.ps1` also creates test executables, screenshots and intermediate files in `build/`. These files can be deleted and regenerated.

## Project layout

```text
src/       C++ game logic, rendering and platform adapters
tests/     Rule, combat, pathfinding and browser tests
tools/     WebAssembly bundling and test-frame conversion tools
build/     Local build output (not committed to Git)
```

## Note

This project is a fictional parody. Its characters, dialogue and events are used for game presentation and do not represent the real statements or views of any person.

## License

The code is available under the [MIT License](LICENSE).
