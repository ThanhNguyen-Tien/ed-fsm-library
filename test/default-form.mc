Project{name:'test-console'}
BaudRate{Bauds:115200}

Section{name:'Blink'}
IntField{name:'Interval', min: 10, max: 1000, readonly:false, command: 100}
DoubleButton{nameLeft: 'Start',nameRight: 'Stop',commandLeft:101, commandRight: 102}
DoubleButton{nameLeft: 'Reg_Sub_A',nameRight: 'Reg_Sub_B',commandLeft:103, commandRight: 104}

Section{name:'Test Value'}
Plot{name:'Sine', color:'cyan',channel:0,scale: 0.3, offset: 600}
Plot{name:'Cosine', color:'magenta',channel:1,scale: 0.3, offset: 600}

Section{name:'Plot'}
Plot{name:'Testvalue_1', color:'yellow',channel:2,scale: 2, offset: 200}
Plot{name:'Testvalue_2', color:'blue',channel:3,scale: 2, offset: 400}

Section{name:'Hello world'}
TextField{name:'Name', readonly:false, command: 105}
Button{name: 'Say Hello' ,command:106}

