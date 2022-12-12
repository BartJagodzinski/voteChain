## Libraries

**sha256** → [https://github.com/okdshin/PicoSHA2](https://github.com/okdshin/PicoSHA2)   
**json** → [https://github.com/nlohmann/json](https://github.com/nlohmann/json)   
**boost** → [www.boost.org](https://www.boost.org/)   

### Mempool
Mempool is constantly listening for new votes until deadline is reached. Deadline is the end of voting time set before start.    
When reached, mempool closes for new votes and waits for the accumulated pool of votes to be closed in blocks.   
   
![mempool](img/mempool.PNG)   
    
### Mining
Mining is about finding nonce. Node must encapsulate accumulated votes in block by hashing block header which includes:   
    
<**block header**>   
- id
- merkle root hash of all votes in block
- previous block hash
- timestamp

</**block header**>    
    
But if we hash that header output will always be the same, that is why we need nonce.     
Node increments nonce until hash of block matches target.     

    
![mining](img/mining.PNG)   
![p2p](img/p2p.PNG)   
![sendBlock](img/sendingBlock.PNG)   
![sendBlock](img/merkleRootHash.PNG)   
