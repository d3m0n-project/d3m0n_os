# d3m0n shell
![shell](https://github.com/d3m0n-project/d3m0n_os/assets/71982379/a4c8e3fc-0ca6-402c-8616-cfd897aa819e)

d3m0n shell is a debug & administration tool that help to manage your d3m0n phone.

## Configuration
d3m0n shell by defaults runs on port 3845, to connect to it use command `telnet <your IP> <port>`, to change it edit config file [located here](https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/config)



## main commands:
| command                        | permissions | description                      |
|--------------------------------|-------------|----------------------------------|
| help                           | none        | display this list                |
| exit                           | none        | quit                             |
| config [get/set] name (value)  | admin       | config your phone                |
| load <path>                    | none        | loads new app                    |
| list                           | none        | list currently loaded apps       |
| run <package>                  | none        | run app by package name          |
| update [all/package_installer] | none        | update d3m0n OS components       |
| fetch <package>                | none        | download package from repository |
| clear                          | none        | clear console                    |
| clean                          | none        | clean scripts & applications     |
| version                        | none        | get current version              |
| display [clear/draw]           | none        | manual display debug commands    |
| info                           | none        | get debug informations           |

