# tpn - Telegram Process Notifier

Executes input from stdin in the background, and messages you on telegram once it completes.

## Usage
Depends on libcurl

Register a bot on telegram, edit the `TELEGRAM_ID` and `BOT_TOKEN` values in `tpn.c`

Build with `gcc tpn.c -lcurl -o tpn`
