# d3m0n OS TODO list

- [x] Add dynamic layout, with possiblity to change layout using script.
	- [x] Could use small layout file as templates and load them dynamically using scripts
	- [ ] Add multiple windows per app
- [x] System config with clock preference
- [ ] Add system theme config (light/dark)

- [x] Handle clicks
	- [ ] Deny click in topbar

- [ ] Fix ui system
	- [x] Locations need to follow real control position
	- [ ] Allow user to customize topbar
	- [ ] Set default auto size if none set
	- [ ] Allow calc(eval) inside of layouts like `calc(100% - 340px - 5px * 2)` + DO `p_client_size`

- [ ] Optimisations
	- [ ] Load all icons at startup for faster display (keep in memory)


- [ ] Docs
	- [ ] Usage docs
		- [ ] Add a how to create an app, provide tools & examples
		- [ ] Fix all docs & academy

Docs:
	window events array: `index 0 = close_window` if app is not launcher