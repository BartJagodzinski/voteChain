## Libraries

**sha256** → [https://github.com/okdshin/PicoSHA2](https://github.com/okdshin/PicoSHA2)   
**json** → [https://github.com/nlohmann/json](https://github.com/nlohmann/json)   
**boost** → [www.boost.org](https://www.boost.org/)   

### Peer-to-peer   
A peer-to-peer (P2P) network is a decentralized type of network in which each participant can act as both a client and a server.   
This means that each node is able to communicate directly with other nodes and share resources (in this case blocks),   
without the need for a central server or authority that controls the network or manages the flow of information.      
Instead, each node is equal and has the same rights (just like voting).  
This makes P2P architecture highly resilient, as there is no single point of failure that can bring down the entire network.   
   
![p2p](img/p2p.PNG)   
   
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
    
But if we hash that header output will always be the same, that's why we need nonce.     
Node increments nonce until hash of block matches target.     

    
![mining](img/mining.PNG)   
   
### Block mined, what's next?   
After finding the correct nonce and mining the block, node must send it to its peers.   
   
![sendBlock](img/sendingBlock.PNG)   
   
![sendBlock](img/merkleRootHash.PNG)   
