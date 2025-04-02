Project{name:'test-event-task'}
BaudRate{Bauds:115200}

Section{name:'M_TASK'}
IntField{name:'Interval', min: 10, max: 2000, readonly:false, command: 100}
IntField{name:'Repeat', min: 1, max: 10, readonly:false, command: 101}
DoubleButton{nameLeft: 'Start',nameRight: 'Stop',commandLeft:102, commandRight: 103}
Button{name: 'Stop Forever' ,command:104}

Section{name:'M_EVENT'}
Button{name: 'Send' ,command:105}