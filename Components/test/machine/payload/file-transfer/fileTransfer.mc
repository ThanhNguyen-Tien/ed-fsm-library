Project{name:'File-Transfer-Test'}

Section{name:'Control Pannel'}
Button{name: 'Start' ,command:100}

Section{name:'Fake Infor'}
TextField{name:'FileName', readonly:false, command: 101}
IntField{name:'BlockSize', min: 0, max: 255, readonly:false, command: 102}
IntField{name:'NumOfBlock', min: 0, max: 255, readonly:false, command: 103}
