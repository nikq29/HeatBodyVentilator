# 📋 Release-Prozess für ESP32 Server Project

Diese Anleitung beschreibt, wie Sie neue stabile Versionen Ihres Projekts veröffentlichen.

## 🎯 Release-Strategie

### Branch-Struktur
- **`main`**: Haupt-Entwicklungs-Branch (privat)
- **`release`**: Stabile Release-Versionen (kann öffentlich gemacht werden)

### Versions-Schema
Wir verwenden [Semantic Versioning](https://semver.org/):
- **MAJOR.MINOR.PATCH** (z.B. 1.0.0)
  - **MAJOR**: Breaking Changes (1.0.0 → 2.0.0)
  - **MINOR**: Neue Features (1.0.0 → 1.1.0)
  - **PATCH**: Bugfixes (1.0.0 → 1.0.1)

## 🚀 Release erstellen

### Schritt 1: Code vorbereiten
```bash
# Stellen Sie sicher, dass Sie auf dem main Branch sind
git checkout main

# Alle Änderungen commiten
git add .
git commit -m "Prepare release v1.0.0"

# Zum release Branch wechseln
git checkout release

# Änderungen von main mergen
git merge main
```

### Schritt 2: Version aktualisieren
Aktualisieren Sie die Version in:
- `platformio.ini` (falls vorhanden)
- `README_PUBLIC.md` im Changelog
- `src/Config.h` in `FIRMWARE_VERSION`

### Schritt 3: Release committen
```bash
git add .
git commit -m "Release v1.0.0"
```

### Schritt 4: Git Tag erstellen
```bash
# Tag erstellen
git tag -a v1.0.0 -m "Release Version 1.0.0 - Beschreibung der Änderungen"

# Tag auf GitHub pushen
git push origin release
git push origin v1.0.0
```

### Schritt 5: GitHub Release erstellen
1. Gehen Sie zu: https://github.com/smarthome-assistant/ESP32_Server_Project_Core/releases
2. Klicken Sie auf "Create a new release"
3. Wählen Sie den Tag: `v1.0.0`
4. Release Title: `v1.0.0 - Kurzbeschreibung`
5. Beschreibung mit Changelog einfügen
6. Optional: Compiled Binaries anhängen (`.bin` Dateien)
7. Klicken Sie auf "Publish release"

## 📝 Release-Checklist

Vor jedem Release:
- [ ] Alle Tests durchgeführt
- [ ] README_PUBLIC.md aktualisiert
- [ ] Changelog erweitert
- [ ] Keine sensiblen Daten im Code
- [ ] Version in Config.h aktualisiert
- [ ] Erfolgreich kompiliert
- [ ] Auf Hardware getestet

## 📄 Changelog-Format

```markdown
### Version X.Y.Z (YYYY-MM-DD)

#### ✨ Neue Features
- Feature 1
- Feature 2

#### 🐛 Bugfixes
- Fix 1
- Fix 2

#### 🔧 Verbesserungen
- Verbesserung 1
- Verbesserung 2

#### ⚠️ Breaking Changes
- Breaking Change 1
```

## 🔄 Schnell-Kommandos

### Neues Minor Release (Feature-Update)
```bash
git checkout main
git pull
# ... Entwicklung ...
git add .
git commit -m "Add new feature"
git checkout release
git merge main
# Version updaten (z.B. 1.0.0 → 1.1.0)
git add .
git commit -m "Release v1.1.0"
git tag -a v1.1.0 -m "Release v1.1.0 - Feature Update"
git push origin release
git push origin v1.1.0
```

### Neues Patch Release (Bugfix)
```bash
git checkout main
git pull
# ... Bugfix ...
git add .
git commit -m "Fix critical bug"
git checkout release
git merge main
# Version updaten (z.B. 1.1.0 → 1.1.1)
git add .
git commit -m "Release v1.1.1"
git tag -a v1.1.1 -m "Release v1.1.1 - Bugfix"
git push origin release
git push origin v1.1.1
```

## 🏷️ Tag-Verwaltung

### Tags anzeigen
```bash
git tag
```

### Tag löschen (lokal)
```bash
git tag -d v1.0.0
```

### Tag löschen (remote)
```bash
git push origin --delete v1.0.0
```

### Zu einem Tag wechseln
```bash
git checkout v1.0.0
```

## 🌐 Öffentliches Repository erstellen

### Option 1: Repository öffentlich machen
1. Gehen Sie zu Repository Settings
2. Scrollen Sie zu "Danger Zone"
3. "Change visibility" → "Make public"
4. **ACHTUNG**: Komplette Git-Historie wird sichtbar!

### Option 2: Neues öffentliches Repository (Empfohlen)
1. Erstellen Sie ein neues Repository auf GitHub (öffentlich)
2. Fügen Sie nur den `release` Branch hinzu:
```bash
cd /pfad/zum/projekt
git remote add public https://github.com/smarthome-assistant/ESP32_Server_Public.git
git push public release:main
git push public --tags
```

## 📦 Compiled Binaries bereitstellen

Für Benutzer ohne Entwicklungsumgebung:

### Binaries kompilieren
```bash
# In PlatformIO
pio run

# Binary findet sich in:
# .pio/build/m5stack-atom/firmware.bin
```

### Binaries zum Release hinzufügen
1. Öffnen Sie den GitHub Release
2. "Edit release"
3. Ziehen Sie `firmware.bin` in den "Attach binaries" Bereich
4. Umbenennen zu: `ESP32_Server_v1.0.0.bin`
5. Save

## 🔐 Sicherheits-Tipps

- ❌ **NIEMALS** hart-codierte Passwörter oder API-Keys committen
- ✅ Verwenden Sie Preferences/NVS für sensible Daten
- ✅ Überprüfen Sie vor jedem Release: `git diff`
- ✅ Nutzen Sie die `.gitignore` für sensible Dateien
- ✅ Review aller Änderungen vor dem Release

## 📞 Support

Bei Fragen zum Release-Prozess:
- GitHub Issues: https://github.com/smarthome-assistant/ESP32_Server_Project_Core/issues
- Email: support@smarthome-assistant.info

---

**Letzte Aktualisierung:** 2025-10-21
