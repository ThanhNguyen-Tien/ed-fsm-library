Project{name:'Example'}

Section{name:'Blink'}
IntField{name:'Interval', min: 10, max: 1000, readonly:false, command: 100}
DoubleButton{nameLeft: 'Start',nameRight: 'Stop',commandLeft:101, commandRight: 102}

Section{name:'Plot'}
Plot{name:'Plot1', color:'cyan',channel:0,scale: 0.3, offset: 600}
Plot{name:'Plot2', color:'magenta',channel:1,scale: 0.3, offset: 600}

Section{name:'Oscilloscope'}
Plot{name:'Oscillo 1', color:'yellow',channel:2,scale: 0.3, offset: 50}
Plot{name:'Oscillo 2', color:'blue',channel:3,scale: 0.3, offset: 50}
Plot{name:'Oscillo 3', color:'white',channel:4,scale: 0.3, offset: 400}
Plot{name:'Oscillo 4', color:'green',channel:5,scale: 0.3, offset: 400}

Section{name:'Hello world'}
TextField{name:'Name', readonly:false, command: 103}
Button{name: 'Say Hello' ,command:104}

