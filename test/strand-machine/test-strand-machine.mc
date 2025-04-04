Project{name:'test-strand-machine'}
BaudRate{Bauds:115200}

Section{name:'Control Pannel'}
TextField{name:'Name', readonly:false, command: 100}
DoubleButton{nameLeft: 'Line 1',nameRight: 'Line 2',commandLeft:101, commandRight: 102}
Button{name: 'Clear' ,command:103}

