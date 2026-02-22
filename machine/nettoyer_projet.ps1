# Script de nettoyage pour le projet Qt Machine

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "  NETTOYAGE DU PROJET" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan

$projectPath = "c:\Users\GIGABYTE\Desktop\machine"
Set-Location $projectPath

# Suppression des dossiers de build
Write-Host "`nSuppression des dossiers de build..." -ForegroundColor Yellow
Remove-Item -Recurse -Force ".\build" -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force ".\machine\build" -ErrorAction SilentlyContinue
Write-Host "OK - Dossiers de build supprimes" -ForegroundColor Green

# Suppression des fichiers .user
Write-Host "`nSuppression des fichiers .user..." -ForegroundColor Yellow
Remove-Item "*.user" -Force -ErrorAction SilentlyContinue
Remove-Item "*.user.*" -Force -ErrorAction SilentlyContinue
Write-Host "OK - Fichiers .user supprimes" -ForegroundColor Green

# Suppression des fichiers generes
Write-Host "`nSuppression des fichiers generes..." -ForegroundColor Yellow
Get-ChildItem -Recurse | Where-Object { $_.Name -match "(Makefile|ui_.*\.h|moc_.*\.cpp|qrc_.*\.cpp)" } | Remove-Item -Force -ErrorAction SilentlyContinue
Write-Host "OK - Fichiers generes supprimes" -ForegroundColor Green

Write-Host "`n================================================================" -ForegroundColor Cyan
Write-Host "  NETTOYAGE TERMINE!" -ForegroundColor Green
Write-Host "================================================================" -ForegroundColor Cyan

Write-Host "`nPROCHAINES ETAPES:" -ForegroundColor Yellow
Write-Host "1. Fermez COMPLETEMENT Qt Creator" -ForegroundColor White
Write-Host "2. Rouvrez Qt Creator" -ForegroundColor White
Write-Host "3. Ouvrez le projet: machine.pro" -ForegroundColor White
Write-Host "4. Build > Rebuild All (Ctrl+Shift+B)" -ForegroundColor White
Write-Host "5. Executez le projet (Ctrl+R)" -ForegroundColor White

# Verifier si Qt Creator est en cours
$qtProcess = Get-Process -Name "qtcreator" -ErrorAction SilentlyContinue
if ($qtProcess) {
    Write-Host "`n>>> ATTENTION: Qt Creator est actuellement en cours d'execution!" -ForegroundColor Red
    Write-Host "    Fermez-le COMPLETEMENT avant de continuer." -ForegroundColor Yellow
}

Write-Host "`nAppuyez sur Entree pour fermer..." -ForegroundColor Cyan
Read-Host

