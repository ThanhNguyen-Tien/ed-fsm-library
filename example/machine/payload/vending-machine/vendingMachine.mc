Project{name:'Vending-Machine-Test'}

Section{name:'Control Pannel'}
DoubleButton{nameLeft: 'INS_COIN',nameRight: 'SEL_DRINK',commandLeft:100, commandRight: 101}
DoubleButton{nameLeft: 'DISPENSE',nameRight: 'CANCEL',commandLeft:102, commandRight: 103}

Section{name:'Fake Number'}
IntField{name:'Number', min: 0, max: 255, readonly:false, command: 104}