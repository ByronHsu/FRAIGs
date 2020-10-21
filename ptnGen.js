var fs = require('fs');

function ptnGen(){
   const COUNT = 100;
	for(let i = 1 ; i <= 15 ; i++){
      let output = ``;
		let x = String(i);
		if(i < 10) 
			x = `0` + i;
      let ctx = fs.readFileSync(`./tests.fraig/sim${x}.aag`,'utf8');
      let data = /^aag\s(\d+)\s(\d+)\s(\d+)\s(\d+)\s(\d+)/g.exec(ctx);
      let miloa = [data[1],data[2],data[3],data[4],data[5]];
      for(let t = 0; t < COUNT; t++){
         for(let j = 0; j < miloa[1]; j++){
            let ran = Math.floor(Math.random()*2);
            if(ran == 0)
               output += `0`;
            else 
               output += `1`;
         }
         output += `\n`;
      }
      fs.writeFile(`./tests.fraig/ptn.${x}`, output, function(err) {
         if(err) {
            return console.log(err);
         }
      });
   }
}
ptnGen();