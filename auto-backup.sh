#!/bin/bash
# Auto-backup script für ESP32 Projekt

# Aktuelles Datum für Commit-Message
DATE=$(date +"%Y-%m-%d %H:%M:%S")

# Prüfe ob Änderungen vorhanden sind
if [ -n "$(git status --porcelain)" ]; then
    echo "Änderungen erkannt, erstelle automatischen Backup..."
    
    # Füge alle Änderungen hinzu
    git add .
    
    # Erstelle Commit mit Zeitstempel
    git commit -m "Auto-backup: $DATE"
    
    # Pushe zu GitHub
    git push
    
    echo "✅ Automatischer Backup erfolgreich: $DATE"
else
    echo "Keine Änderungen erkannt."
fi
