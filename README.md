# `pd.lv2`

Simple LV2 plugin that allows to use pure data (via libpd) to implement and edit filters (audio to audio) and instruments (midi to audio) in pure data in real time.

Current state is WIP, you will probably struggle to use it reliably. Feel free to fork or open a PR.

Supports mono (current .ttl entry may be broken) and stereo, if you need more channels, edit `gen-plugins.sh` and add a new entry to `plugin.ttl`

All patches are stored in `~/lv2-pd` with randomized names. (Storing them in lv2 state is tedious, hosts may save state at any time which requires either reopening a patch (which is annoying and recent changes may not be saved) or dynamic reloading (may be just impossible with libpd))

Tested with Ardour 8, it somewhat works.

## Dependencies
- Installation of pd gui at `/usr/lib/pd`
- `libpd`

## Build
You'd need libpd headers (Arch's `pd` package includes them) and lv2 headers (for Arch there's `lv2` package).
```sh
# this will install the built plugin to ~/.lv2/lv2-pd
./build.sh
```
See `gen-plugins.sh` and `build.sh` for more configuration options

## Similar projects
- [Camomile](https://github.com/pierreguillot/Camomile) — packs pd patches into lv2 plugins.
- [x37v/pdlv2](https://github.com/x37v/pdlv2/) — also packs pd patches into lv2 plugins.
- [plugdata](https://github.com/plugdata-team/plugdata) — similar to what this plugin does but with its own UI.
- [agraef/pd-lv2plugin](https://bitbucket.org/agraef/pd-lv2plugin) — this one goes the other way: allows to use lv2 plugins inside pd patches.

## TODO
- Windows support (not me).
- `lv2:showInterface` so gui window could be properly opened and hidden.
- Figure out ttl stuff so hosts could recognize it as an instrument plugin (now in ardour you need to manually route pins).
- Polish code, remove memory leaks (there's probably a bunch).
- Use lv2 logger instead of printing to stdout (optional feature?).
