# Build script for Windows
# Usage: .\build.ps1

Write-Host "Building jsonrepair-cpp..." -ForegroundColor Green

# Create build directory
if (Test-Path build) {
    Write-Host "Cleaning existing build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force build
}

New-Item -ItemType Directory -Path build | Out-Null
Set-Location build

# Configure with CMake
Write-Host "`nConfiguring with CMake..." -ForegroundColor Green
cmake ..

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Build
Write-Host "`nBuilding..." -ForegroundColor Green
cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Run tests
Write-Host "`nRunning tests..." -ForegroundColor Green
ctest -C Release --output-on-failure

if ($LASTEXITCODE -ne 0) {
    Write-Host "Tests failed!" -ForegroundColor Red
    Set-Location ..
    exit 1
}

# Run example
Write-Host "`nRunning example..." -ForegroundColor Green
Write-Host "=" -ForegroundColor Cyan -NoNewline; Write-Host ("=" * 70) -ForegroundColor Cyan
.\Release\jsonrepair_example.exe
Write-Host "=" -ForegroundColor Cyan -NoNewline; Write-Host ("=" * 70) -ForegroundColor Cyan

Set-Location ..

Write-Host "`nBuild completed successfully!" -ForegroundColor Green
Write-Host "Binaries are in: .\build\Release\" -ForegroundColor Yellow
