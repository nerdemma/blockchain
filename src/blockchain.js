const SHA256 = require("crypto-js/sha256");
const Block = require("./block");

class Blockchain 
{
	constructor(){
	this.chain = [];
	this.height = -1;	
	this.initializeChain();	
}

	async initializeChain()
		{
			if(this.height === -1)
			{
			const block = new Block({ data: "Genesis block" });
			await this.addBlock(block);	
			}		
		}

async addBlock(block)
		{
		block.height = this.chain.length;
		block.time = new Date().getTime().toString();

			if(this.chain.length > 0)
			{
			block.previousBlockHash = this.chain[this.chain.length - 1].hash;
			}
			const errors = await this.validateChain(); 
	
			if(errors.lenght > 0)
			{
			throw new error("The chain is not valid", errors);
			}
	
			block.hash = SHA256(JSON.stringify(block)).toString();
			this.chain.push(block);
		}
	
	
	validateChain()
		{
		let self = this;
		const errors = [];
		return new Promise(async (resolve, reject) => {
		self.chain.map(async (block) => {
		
				try{
					let isvalid = await block.validate();
					if(!isvalid)
					{
					errors.push(new Error('the block ${block.heigth} is not valid'));
					}
					}
		catch(err){
		errors.push(err);
					}
					
				});
			resolve(errors);
			});
		}


print()
	{
	let self = this;
	for(let block of self.chain)
	{
	console.log(block.toString());
	}
	}

}
module.exports = Blockchain;
