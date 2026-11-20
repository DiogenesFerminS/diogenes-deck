#!/usr/bin/env python3

import shutil
import subprocess
import sys
import time
import serial

SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 115200
RECONNECT_DELAY_SEC = 2.0

OPENCODE_BIN = (
    shutil.which("opencode")
    or "/home/diogenesfermin/.nvm/versions/node/v24.19.0/bin/opencode"
)

ACTIONS: dict[str, tuple[str, list[str]]] = {
    "CMD:TERMINAL": ("Terminal", ["gnome-terminal"]),
    "CMD:BROWSER": ("Browser", ["firefox"]),
    "CMD:CODE": ("VS Code", ["code"]),
    "CMD:OPENCODE": ("Opencode", ["gnome-terminal", "--", OPENCODE_BIN]),
    "CMD:ANTIGRAVITY": ("Antigravity", ["/usr/bin/antigravity-ide"]),
    "CMD:LOCK": (
        "Locked",
        [
            "sh",
            "-c",
            "loginctl lock-session && gdbus call --session --dest org.gnome.ScreenSaver --object-path /org/gnome/ScreenSaver --method org.gnome.ScreenSaver.SetActive true",
        ],
    ),
    "CMD:WAKE_PC": (
        "Wake PC",
        [
            "sh",
            "-c",
            "loginctl unlock-session; gdbus call --session --dest org.gnome.ScreenSaver --object-path /org/gnome/ScreenSaver --method org.gnome.ScreenSaver.SetActive false",
        ],
    ),
    "CMD:SUSPEND": ("Suspend", ["systemctl", "suspend"]),
    "CMD:VOL_UP": ("Vol +", ["wpctl", "set-volume", "-l", "1.0", "@DEFAULT_AUDIO_SINK@", "5%+"]),
    "CMD:VOL_DOWN": ("Vol -", ["wpctl", "set-volume", "@DEFAULT_AUDIO_SINK@", "5%-"]),
    "CMD:MUTE": ("Mute", ["wpctl", "set-mute", "@DEFAULT_AUDIO_SINK@", "toggle"]),
    "CMD:REBOOT": ("Reboot", ["systemctl", "reboot"]),
}


def launch_command(cmd: list[str]) -> bool:
    try:
        subprocess.Popen(
            cmd,
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            start_new_session=True,
        )
        return True
    except Exception as exc:
        print(f"[ERROR] Failed to launch {cmd}: {exc}", file=sys.stderr)
        return False


def run_daemon(port: str = SERIAL_PORT, baud: int = BAUD_RATE) -> None:
    print(f"[DECK] Listening for Diogenes Deck on {port} @ {baud} baud...")
    while True:
        try:
            with serial.Serial(port, baud, timeout=1.0) as ser:
                print(f"[DECK] Connected to {port}")
                ser.write(b"ACK:Connected\n")
                while True:
                    raw = ser.readline()
                    if not raw:
                        continue
                    line = raw.decode("utf-8", errors="ignore").strip()
                    if not line:
                        continue

                    print(f"[SERIAL] {line}")
                    if line in ACTIONS:
                        label, cmd = ACTIONS[line]
                        if launch_command(cmd):
                            ser.write(f"ACK:OK {label}\n".encode("utf-8"))
        except serial.SerialException:
            time.sleep(RECONNECT_DELAY_SEC)
        except KeyboardInterrupt:
            print("\n[DECK] Stopping daemon.")
            break


if __name__ == "__main__":
    target_port = sys.argv[1] if len(sys.argv) > 1 else SERIAL_PORT
    run_daemon(target_port)
