# PowerShell File Watcher für automatische Git Commits
# Überwacht Änderungen im src/ und data/ Verzeichnis

$projectPath = Get-Location
$srcPath = Join-Path $projectPath "src"
$dataPath = Join-Path $projectPath "data"

Write-Host "🔍 Starte File Watcher für automatische Git Commits..." -ForegroundColor Green
Write-Host "📁 Überwache: $srcPath und $dataPath" -ForegroundColor Cyan
Write-Host "⏹️  Drücke Strg+C zum Beenden" -ForegroundColor Yellow

# File System Watcher für src/
$srcWatcher = New-Object System.IO.FileSystemWatcher
$srcWatcher.Path = $srcPath
$srcWatcher.IncludeSubdirectories = $true
$srcWatcher.EnableRaisingEvents = $true

# File System Watcher für data/
$dataWatcher = New-Object System.IO.FileSystemWatcher
$dataWatcher.Path = $dataPath
$dataWatcher.IncludeSubdirectories = $true
$dataWatcher.EnableRaisingEvents = $true

# Timer für verzögerte Commits (sammelt Änderungen 30 Sekunden)
$timer = New-Object System.Timers.Timer
$timer.Interval = 30000 # 30 Sekunden
$timer.AutoReset = $false

# Event Handler für Änderungen
$action = {
    $path = $Event.SourceEventArgs.FullPath
    $changeType = $Event.SourceEventArgs.ChangeType
    $fileName = Split-Path $path -Leaf
    
    # Ignoriere temporäre und Build-Dateien
    if ($fileName -match '\.(tmp|bak|log)$' -or $path -match '\.pio\\') {
        return
    }
    
    Write-Host "📝 Änderung erkannt: $fileName ($changeType)" -ForegroundColor Yellow
    
    # Restart timer (sammelt mehrere Änderungen)
    $timer.Stop()
    $timer.Start()
}

# Timer Event Handler für Git Commit
$timerAction = {
    $date = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    
    try {
        # Prüfe auf Änderungen
        $status = git status --porcelain
        
        if ($status) {
            Write-Host "🔄 Erstelle automatischen Commit..." -ForegroundColor Green
            
            git add .
            git commit -m "Auto-save: $date"
            git push
            
            Write-Host "✅ Automatischer Commit erfolgreich: $date" -ForegroundColor Green
        }
    }
    catch {
        Write-Host "❌ Fehler beim Commit: $($_.Exception.Message)" -ForegroundColor Red
    }
}

# Event Handlers registrieren
Register-ObjectEvent -InputObject $srcWatcher -EventName "Changed" -Action $action
Register-ObjectEvent -InputObject $srcWatcher -EventName "Created" -Action $action
Register-ObjectEvent -InputObject $srcWatcher -EventName "Deleted" -Action $action

Register-ObjectEvent -InputObject $dataWatcher -EventName "Changed" -Action $action
Register-ObjectEvent -InputObject $dataWatcher -EventName "Created" -Action $action
Register-ObjectEvent -InputObject $dataWatcher -EventName "Deleted" -Action $action

Register-ObjectEvent -InputObject $timer -EventName "Elapsed" -Action $timerAction

try {
    while ($true) {
        Start-Sleep -Seconds 1
    }
}
finally {
    # Cleanup
    $srcWatcher.Dispose()
    $dataWatcher.Dispose()
    $timer.Dispose()
    Get-EventSubscriber | Unregister-Event
}
