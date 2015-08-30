# B+ Tree

## Quick Intro
A B/B+ tree is a common data structure used when dealing with an amount of data that can't be stored in RAM. Since this data would traditionally be stored in a hard disk, each disk access was significantly more costly than a RAM access. In order to alleviate this problem, B trees were made to minimize the number of disk accesses. [Click here for more info.](https://en.wikipedia.org/wiki/B-tree)

## Difference Between B and B+ Tree
One stores data along with keys in its internal nodes (B-Tree) and the other only has keys in its internal nodes. There are use cases for both structures, but the B+ tree tends to be more used.

In the case of this implementation, there are some slight variations compared to a traditional one. The internal nodes keep track of a little data: the minimums of its children nodes.

## About this Implementation
There are extra files used to test the ones in this repo, but I do not have permission to use them. However, considering I wrote this to learn more about the B-Tree data structure, there are better implementations out there than mine. Use this more as a reference point for your own learning.

#### Special thanks to my partner Chris Needham for doing all the typing.
