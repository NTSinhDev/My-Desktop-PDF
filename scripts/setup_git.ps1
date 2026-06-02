# Run from repo root after installing GitHub CLI
param(
  [string]$RemoteName = "my-desktop-pdf"
)

Set-Location $PSScriptRoot\..

if (-not (Test-Path .git)) {
  git init -b main
}

git add -A
git status

Write-Host ""
Write-Host "Create GitHub repo (public):"
Write-Host "  gh repo create $RemoteName --public --source=. --remote=origin --description `"My Desktop PDF - offline Qt6/PDFium reader`" --push"
