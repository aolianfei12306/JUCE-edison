# One-Click Setup Script for Windows
# This script auto-installs all build dependencies with zero manual browsing.
# Run in PowerShell (as Administrator):
#   Set-ExecutionPolicy Bypass -Scope Process -Force; .\setup.ps1

param(
    [switch]$NoMSYS2  # Skip MSYS2/MinGW installation, use Chocolatey instead
)

Write-Host "=== Open Edison - Windows Dependencies Installer ===" -ForegroundColor Cyan

# ── Helper: Check if running as admin ──
$isAdmin = ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if (-not $isAdmin) {
    Write-Warning "Some installers require Administrator privileges. If things fail, re-run as Administrator."
}

# ── Detect package managers ──
$hasWinget = $null -ne (Get-Command winget -ErrorAction SilentlyContinue)
$hasChoco  = $null -ne (Get-Command choco -ErrorAction SilentlyContinue)
$hasScoop  = $null -ne (Get-Command scoop -ErrorAction SilentlyContinue)

# ── Strategy ──
if ($hasWinget -and -not $NoMSYS2) {
    # Strategy A: winget + MSYS2 (recommended, lightweight)
    Write-Host "`n[1/2] Installing MSYS2 via winget..." -ForegroundColor Yellow
    winget install MSYS2.MSYS2 --accept-package-agreements --accept-source-agreements 2>$null
    if ($LASTEXITCODE -ne 0 -and $LASTEXITCODE -ne -1978335189) {
        Write-Warning "winget install failed (exit $LASTEXITCODE). Trying Chocolatey instead..."
        $hasWinget = $false
    } else {
        $msys2Root = "C:\tools\msys64"
        if (-not (Test-Path $msys2Root)) { $msys2Root = "$env:LOCALAPPDATA\msys64" }
        if (-not (Test-Path $msys2Root)) { $msys2Root = "$env:ProgramFiles\msys2" }

        Write-Host "`n[2/2] Installing GCC + CMake + Ninja via MSYS2 pacman..." -ForegroundColor Yellow
        & "$msys2Root\usr\bin\pacman.exe" -S --noconfirm --needed `
            mingw-w64-x86_64-gcc `
            mingw-w64-x86_64-cmake `
            mingw-w64-x86_64-ninja `
            mingw-w64-x86_64-pkg-config `
            git 2>$null

        Write-Host "`n✅ Done!" -ForegroundColor Green
        Write-Host "`nNow open 'MSYS2 MinGW64' from Start Menu and run:" -ForegroundColor Cyan
        Write-Host "  cd /path/to/JUCE-edison" -ForegroundColor White
        Write-Host "  cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release" -ForegroundColor White
        Write-Host "  cmake --build build" -ForegroundColor White
        exit 0
    }
}

if ($hasChoco -or $hasScoop) {
    # Strategy B: Chocolatey or Scoop (lighter than VS Build Tools)
    $pkgMgr = if ($hasChoco) { "choco" } else { "scoop" }
    Write-Host "`nInstalling via $pkgMgr ..." -ForegroundColor Yellow

    if ($hasChoco) {
        choco install mingw cmake ninja git -y --no-progress 2>$null
    } else {
        scoop install gcc cmake ninja git 2>$null
    }

    Write-Host "`n✅ Done!" -ForegroundColor Green
    Write-Host "`nNow in any terminal:" -ForegroundColor Cyan
    Write-Host "  cd JUCE-edison" -ForegroundColor White
    Write-Host "  cmake -B build -G ""MinGW Makefiles"" -DCMAKE_BUILD_TYPE=Release" -ForegroundColor White
    Write-Host "  cmake --build build" -ForegroundColor White
    exit 0
}

# ── Nothing available → install Chocolatey first ──
Write-Host "`nNo package manager detected. Installing Chocolatey..." -ForegroundColor Yellow
Set-ExecutionPolicy Bypass -Scope Process -Force
[System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072
Invoke-Expression ((New-Object System.Net.WebRequest).Invoke('https://community.chocolatey.org/install.ps1'))

Write-Host "`nInstalling GCC + CMake + Ninja + Git via Chocolatey..." -ForegroundColor Yellow
choco install mingw cmake ninja git -y --no-progress 2>$null

Write-Host "`n✅ Done!" -ForegroundColor Green
Write-Host "Restart your terminal, then:" -ForegroundColor Cyan
Write-Host "  cd JUCE-edison" -ForegroundColor White
Write-Host "  cmake -B build -G ""MinGW Makefiles"" -DCMAKE_BUILD_TYPE=Release" -ForegroundColor White
Write-Host "  cmake --build build" -ForegroundColor White
