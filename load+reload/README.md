# Load+Reload

A  proof of concept of [Load+Reload](https://mlq.me/download/takeaway.pdf).
This side channel attack takes advantage
of how accessing two virtual addresses
mapped to the same physical address
in succession makes the processor
fallback to the L2 cache
in recent AMD architectures.
