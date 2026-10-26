# Project One: Runtime and Memory Analysis

## CS 300 - Data Structures and Algorithms

The ABCU advising program required course information to be loaded, stored, searched, and displayed. During Project One, I compared three data structures for storing the course information: a vector, a hash table, and a binary search tree. Each structure provides different advantages in runtime performance, memory usage, searching, and displaying the courses in alphanumeric order.

## Vector

A vector stores course objects sequentially in memory.

### Runtime

- Loading all `n` courses into the vector requires approximately **O(n)** time when appending each course.
- Searching for a specific course requires a linear search and has a worst-case runtime of **O(n)**.
- If the courses must be displayed in alphanumeric order, the vector must first be sorted. A comparison-based sorting algorithm generally requires **O(n log n)** time.

### Memory

A vector requires **O(n)** memory because one course object is stored for each course in the input file. A vector may also reserve additional capacity as it grows, but its overall memory requirement remains proportional to the number of courses.

### Advantages and Disadvantages

The vector is simple to implement and provides efficient sequential access. However, searching for a particular course becomes slower as the number of courses increases unless the vector is sorted and a more efficient search technique is used.

---

## Hash Table

A hash table stores each course according to a hash value calculated from its course number.

### Runtime

- Loading `n` courses has an average runtime of approximately **O(n)** because insertion is normally **O(1)** per course.
- Searching for a course has an average runtime of **O(1)**.
- In the worst case, many courses could hash to the same location, causing search or insertion to degrade to **O(n)**.
- A hash table does not naturally maintain alphanumeric order. To print all courses in sorted order, the courses would need to be collected and sorted, requiring approximately **O(n log n)** time.

### Memory

The hash table requires **O(n)** memory for the course records plus additional memory for the table's buckets and any collision-handling structures.

### Advantages and Disadvantages

The primary advantage of a hash table is its very efficient average search time. This makes it a strong option when quickly locating an individual course is the most important requirement. Its disadvantage for this project is that it does not automatically keep the courses in sorted order.

---

## Binary Search Tree

A binary search tree stores courses according to their course numbers. Course numbers smaller than a node are placed in the left subtree, while larger course numbers are placed in the right subtree.

### Runtime

- In a reasonably balanced binary search tree, insertion and search require approximately **O(log n)** time.
- Loading `n` courses therefore requires approximately **O(n log n)** time in the average case.
- If the tree becomes completely unbalanced, insertion and searching can degrade to **O(n)** per operation, with a worst-case load time of **O(n²)**.
- Printing all courses using an inorder traversal requires **O(n)** time and automatically produces the courses in alphanumeric order.

### Memory

The binary search tree requires **O(n)** memory because each course is stored in a node. Each node also requires pointers to its left and right children. Recursive traversal may require additional memory based on the height of the tree.

### Advantages and Disadvantages

The binary search tree provides efficient searching when the tree is reasonably balanced and makes it easy to display the entire course list in alphanumeric order through an inorder traversal. Its main disadvantage is that performance can decrease if the tree becomes highly unbalanced.

---

## Comparison

| Data Structure | Load / Insert | Search | Sorted Course Output | Memory |
|---|---|---|---|---|
| Vector | O(n) total | O(n) | O(n log n) if sorting is required | O(n) |
| Hash Table | O(n) average total | O(1) average | O(n log n) because sorting is required | O(n) plus table overhead |
| Binary Search Tree | O(n log n) average total | O(log n) average | O(n) using inorder traversal | O(n) plus node pointers |

## Recommendation

For the ABCU advising program, I selected the **binary search tree**.

Although a hash table offers faster average lookup performance, the advising program needs both individual course searches and an alphanumeric list of all courses. A binary search tree supports both requirements well. Searching is efficient when the tree is reasonably balanced, and an inorder traversal naturally prints the courses in sorted order without requiring a separate sorting operation.

This comparison showed me that selecting a data structure should be based on the complete set of program requirements rather than choosing the structure with the fastest performance for only one operation.
