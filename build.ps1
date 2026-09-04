param([switch]$Native)
$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Path build -Force | Out-Null
    & clang++ --target=wasm32 -std=c++17 -O2 -ffreestanding -fno-exceptions -fno-rtti -nostdlib -fno-builtin src/game.cpp '-Wl,--no-entry,--export-all,--allow-undefined,--initial-memory=33554432,-z,stack-size=1048576' -o build/desert-goat.wasm
    if ($LASTEXITCODE -ne 0) { throw 'WebAssembly compilation failed.' }
    & node tools/bundle.cjs
    if ($LASTEXITCODE -ne 0) { throw 'Browser bundle generation failed.' }
    & g++ -std=c++17 -O2 tests/cpp_tests.cpp -o build/cpp_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'C++ test compilation failed.' }
    & ./build/cpp_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'C++ tests failed.' }
    & g++ -std=c++17 -O2 tests/combat_tests.cpp -o build/combat_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Combat test compilation failed.' }
    & ./build/combat_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Combat and cinematic tests failed.' }
    & g++ -std=c++17 -O2 tests/menu_warning_tests.cpp -o build/menu_warning_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Menu/warning test compilation failed.' }
    & ./build/menu_warning_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Menu, warning, seal and victory tests failed.' }
    & g++ -std=c++17 -O2 tests/pathfinding_tests.cpp -o build/pathfinding_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Pathfinding test compilation failed.' }
    & ./build/pathfinding_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'IDA* shortest-path tests failed.' }
    & node --test tests/wasm.test.cjs tests/browser_adapter.test.cjs
    if ($LASTEXITCODE -ne 0) { throw 'WebAssembly integration tests failed.' }
    & node tools/render-frames.cjs
    & g++ -std=c++17 -O2 tests/campaign_tests.cpp -o build/campaign_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Campaign test compilation failed.' }
    & ./build/campaign_tests.exe
    if ($LASTEXITCODE -ne 0) { throw 'Campaign playthrough failed.' }
    if ($Native) {
        & g++ -std=c++17 -O2 -static -mwindows src/windows.cpp -lgdi32 -lsapi -lole32 -loleaut32 -luuid -o build/DesertGoat.exe
        if ($LASTEXITCODE -ne 0) { throw 'Windows build failed.' }
    }
} finally { Pop-Location }
