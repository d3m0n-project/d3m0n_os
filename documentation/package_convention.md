# App package Convention

App packages are formated as follow:
`com.provider_name.module_name.path.to`

example: `com.4re5.d3m0n.system.desktop` (located in `/apps/d3m0n/system/desktop`)

Note that provider name is stored inside of the module_name's folder.

---

## Format
```
/apps/                       (apps root directory)
├─ module1/                  (module name)
│  ├─ category/              (category inside of module)
│  │  ├─ app1/               (app folder)
│  │  │  ├─ layouts/
│  │  │  │  ├─ main.layout
│  │  │  ├─ src/
│  │  │  │  ├─ main.src
│  │  │  ├─ app              (app manifest)
│  │  ├─ app2/
│  ├─ provider               (contains the provider name)
├─ module2/
```
