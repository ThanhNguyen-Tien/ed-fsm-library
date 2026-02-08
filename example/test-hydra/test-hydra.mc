Project{name:'Test Hydra'}
Baud{'921600'}

Section{name:'Configuration & Commands'}
Button{name: 'Start' , command:100}
Button{name: 'Stop' , command:101}
DoubleButton{nameLeft: 'Left',nameRight: 'Right',commandLeft:102, commandRight: 103}
IntField{name:'IntField', min: -1000, max: 1000, readonly:false,  command: 104}
TextField{name:'TextField', command: 105}

Plot{name:'Ch0', channel:0,scale: 1, offset: 0}
Plot{name:'Ch1', channel:1,scale: 1, offset: 0}
Plot{name:'Ch2', channel:2,scale: 1, offset: 0}
Plot{name:'Ch3', channel:3,scale: 1, offset: 0}
Plot{name:'Ch4', channel:4,scale: 1, offset: 0}
Plot{name:'Ch5', channel:5,scale: 1, offset: 0}
Plot{name:'Ch6', channel:6,scale: 1, offset: 0}
Plot{name:'Ch7', channel:7,scale: 1, offset: 0}