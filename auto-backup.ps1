# Auto-backup script für ESP32 Projekt (PowerShell)
param(
    [string]$Message = ""
)

# Aktuelles Datum für Commit-Message
$Date = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

# Prüfe ob Änderungen vorhanden sind
$Status = git status --porcelain

if ($Status) {
    Write-Host "Aenderungen erkannt, erstelle automatischen Backup..." -ForegroundColor Yellow
    
    # Füge alle Änderungen hinzu
    git add .
    
    # Verwende custom Message oder Auto-backup mit Zeitstempel
    if ($Message) {
        git commit -m "$Message"
    } else {
        git commit -m "Auto-backup: $Date"
    }
    
    # Pushe zu GitHub
    git push
    
    Write-Host "✅ Automatischer Backup erfolgreich: $Date" -ForegroundColor Green
} else {
    Write-Host "Keine Aenderungen erkannt." -ForegroundColor Gray
}
