## Libraries
     
**Base58** → [https://gist.github.com/miguelmota/ff591873da4f76393ce48efe62d49fd1](https://gist.github.com/miguelmota/ff591873da4f76393ce48efe62d49fd1)       
**secp256k1** → [https://github.com/bitcoin-core/secp256k1](https://github.com/bitcoin-core/secp256k1)     
**sha256** → [https://github.com/okdshin/PicoSHA2](https://github.com/okdshin/PicoSHA2)   
**json** → [https://github.com/nlohmann/json](https://github.com/nlohmann/json)   
**boost** → [https://www.boost.org](https://www.boost.org)     
    

### Usage     
Linux: make      
macOS: make macOS
      
Configure mempool_config.json and checker_config.json     
command: date +%s to get time in unix time, copy it to "deadline" field in config files. Add as many seconds as you want the voting to last and save both files. Now you can run ./checker and ./node.

   
### Mempool
Mempool is constantly listening for new votes (like ballot box) until deadline is reached. Deadline is the end of voting time set before start. When reached, mempool closes for new votes and waits for the accumulated pool of votes to be closed in blocks.   
   
![mempool](img/mempool.PNG)   
    
### Mining
Mining is about finding nonce. Node must encapsulate accumulated votes in block by hashing block header which includes:   
    
<**block header**>   
- id
- merkle root hash of all votes in block
- previous block hash
- timestamp
- nonce

</**block header**>    
    
Node increments nonce until block hash matches target.     

    
![mining](img/mining.PNG)   
   
    
### [Merkle root](https://learnmeabitcoin.com/technical/merkle-root)   
   
![sendBlock](img/merkleRootHash.PNG)   
   
If merkle root hash is not correct block is rejected, otherwise node verify block hash by hashing whole header.    
If block hash is not correct block is rejected, otherwise is added to chain. Node must erase votes of added block from mempool.   

### Fraud protection    
   
![voteChange](img/changeOfVote.PNG)  
    
    
Any change in votes included in block completely changes merkle root hash, which changes block header.   
    
    
![fraud](img/fraud.PNG)   
    
    
Block hash changes completely, meaning that the next block's "previous hash" pointer is invalid and needs to be changed. This causes the headers of all subsequent blocks to change and have to be mined again.
