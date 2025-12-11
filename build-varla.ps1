# Varla Module Build Script for Oblivion Remastered
# This script builds the Varla.dll for OBSE64

param(
    [string]$OblivionPath = "",
    [switch]$AutoCopy = $false,
    [string]$Config = "Release"
)

Write-Host "=== Varla Module Build Script ===" -ForegroundColor Cyan
Write-Host ""

# Set Oblivion path if auto-copy is enabled
if ($AutoCopy) {
    if ($OblivionPath -eq "") {
        # Try to detect Steam installation
        $steamPath = "C:\Steam\steamapps\common\Oblivion Remastered\OblivionRemastered\Binaries\Win64"
        if (Test-Path $steamPath) {
            $OblivionPath = $steamPath
            Write-Host "Detected Oblivion Remastered at: $OblivionPath" -ForegroundColor Green
        } else {
            Write-Host "ERROR: Oblivion path not found. Please specify with -OblivionPath parameter" -ForegroundColor Red
            exit 1
        }
    }
    $env:Oblivion64Path = $OblivionPath
    Write-Host "Auto-copy enabled to: $OblivionPath" -ForegroundColor Yellow
    $copyFlag = "-DOBSE_COPY_OUTPUT=ON"
} else {
    $copyFlag = "-DOBSE_COPY_OUTPUT=OFF"
}

# Configure CMake
Write-Host ""
Write-Host "Configuring CMake..." -ForegroundColor Cyan
cmake -B build -S . $copyFlag

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: CMake configuration failed!" -ForegroundColor Red
    exit 1
}

# Build
Write-Host ""
Write-Host "Building Varla.dll ($Config)..." -ForegroundColor Cyan
cmake --build build --config $Config

if ($LASTEXITCODE -ne 0) {
    Write-Host "ERROR: Build failed!" -ForegroundColor Red
    exit 1
}

# Show output location
$dllPath = "build\obse64\$Config\Varla.dll"
Write-Host ""
Write-Host "=== Build Successful! ===" -ForegroundColor Green
Write-Host "DLL location: $dllPath" -ForegroundColor Yellow

if (Test-Path $dllPath) {
    $dllInfo = Get-Item $dllPath
    Write-Host "DLL size: $([math]::Round($dllInfo.Length / 1KB, 2)) KB" -ForegroundColor Yellow

    if (-not $AutoCopy) {
        Write-Host ""
        Write-Host "To copy to Oblivion Remastered, either:" -ForegroundColor Cyan
        Write-Host "  1. Run this script with -AutoCopy -OblivionPath 'C:\Path\To\Oblivion'" -ForegroundColor White
        Write-Host "  2. Manually copy $dllPath to your Oblivion directory" -ForegroundColor White
    } else {
        Write-Host ""
        Write-Host "DLL automatically copied to: $OblivionPath" -ForegroundColor Green
    }
}

Write-Host ""
Write-Host "Varla module functions available:" -ForegroundColor Cyan
Write-Host "  - VarlaWriteToFile `"filename`" `"content`"" -ForegroundColor White
Write-Host "  - VarlaReadFromFile `"filename`"" -ForegroundColor White
Write-Host ""
