// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043


#ifndef PAIRINGPQ_H
#define PAIRINGPQ_H

#include "Eecs281PQ.h"
#include <deque>
#include <utility>

// A specialized version of the priority queue ADT implemented as a pairing
// heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class PairingPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Each node within the pairing heap
    class Node {
        public:
            // TODO: After you add add one extra pointer (see below), be sure
            //       to initialize it here.
            explicit Node(const TYPE &val): elt{ val }, child{ nullptr }, sibling{ nullptr }, parent{nullptr}
            {}

            // Description: Allows access to the element at that Node's
            //              position. There are two versions, getElt() and a
            //              dereference operator, use whichever one seems
            //              more natural to you.
            // Runtime: O(1) - this has been provided for you.
            const TYPE &getElt() const { return elt; }
            const TYPE &operator*() const { return elt; }

            // The following line allows you to access any private data
            // members of this Node class from within the PairingPQ class.
            // (ie: myNode.elt is a legal statement in PairingPQ's add_node()
            // function).
            friend PairingPQ;

        private:
            TYPE elt;
            Node *child;
            Node *sibling;
            Node *parent;
            // TODO: Add one extra pointer (parent or previous) as desired.
    }; // Node


    // Description: Construct an empty pairing heap with an optional
    //              comparison functor.
    // Runtime: O(1)
    explicit PairingPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
        // TODO: Implement this function.
        count = 0;
        root = nullptr;
    } // PairingPQ()


    // Description: Construct a pairing heap out of an iterator range with an
    //              optional comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    PairingPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, count{0}, root{nullptr}
        {
            for (auto itr = start; itr != end; itr++){
                this->push(*itr);
            }
    } // PairingPQ()


    // Description: Copy constructor.
    // Runtime: O(n)
    PairingPQ(const PairingPQ &other) :
        BaseClass{ other.compare }, count{0},root{nullptr} {
        // TODO: Implement this function.
        // NOTE: The structure does not have to be identical to the original,
        //       but it must still be a valid pairing heap.
        if (other.root == root) exit(1);
        if (other.root == nullptr) return;
        std::deque <Node*> visit;
        visit.push_back(other.root);
        while (!visit.empty()){
            Node* temp = visit.front();
            visit.pop_back();
            if(temp->child != nullptr) visit.push_back(temp->child);
            if(temp->sibling != nullptr) visit.push_back(temp->sibling);
            push(temp->getElt());
        }
        
 
    } // PairingPQ()


    // Description: Copy assignment operator.
    // Runtime: O(n)
    PairingPQ &operator=(const PairingPQ &rhs) {
        // TODO: Implement this function.
        // HINT: Use the copy-swap method from the "Arrays and Containers"
        //       lecture.

        // This line is present only so that this provided file compiles.
        PairingPQ temp(rhs);
        std::swap(this->compare, temp.compare);
        std::swap(this->root, temp.root);
        std::swap(this->count, temp.count);
        return *this;
    } // operator=()


    // Description: Destructor
    // Runtime: O(n)
    ~PairingPQ() {
        // TODO: Implement this function.
        if (root == nullptr) return;
        std::deque <Node*> des;
        des.push_back(this->root);
        while (!des.empty()){
            Node* temp = des.front();
            des.pop_front();
            if(temp->child != nullptr) des.push_back(temp->child);
            if(temp->sibling != nullptr) des.push_back(temp->sibling);
            delete temp;
        }
    } // ~PairingPQ()


    // Description: Assumes that all elements inside the pairing heap are out
    //              of order and 'rebuilds' the pairing heap by fixing the
    //              pairing heap invariant. You CANNOT delete 'old' nodes
    //              and create new ones!
    // Runtime: O(n)
    virtual void updatePriorities() {
        // TODO: Implement this function.
        std::deque <Node*> update;
        update.push_back(root);
        this->root = nullptr;
        while (!update.empty()){
            Node* temp = update.front();
            update.pop_front();
            if (temp->child != nullptr) update.push_back(temp->child);
            if (temp->sibling != nullptr) update.push_back(temp->sibling);
            temp->child = nullptr;
            temp->parent = nullptr;
            temp->sibling = nullptr;
            root = meld(temp, root);
        }
    } // updatePriorities()


    // Description: Add a new element to the pairing heap. This is already
    //              done. You should implement push functionality entirely in
    //              the addNode() function, and this function calls
    //              addNode().
    // Runtime: O(1)
    virtual void push(const TYPE &val) {
        addNode(val);
    } // push()


    // Description: Remove the most extreme (defined by 'compare') element
    //              from the pairing heap.
    // Note: We will not run tests on your code that would require it to pop
    //       an element when the pairing heap is empty. Though you are
    //       welcome to if you are familiar with them, you do not need to use
    //       exceptions in this project.
    // Runtime: Amortized O(log(n))
    virtual void pop() {
        // TODO: Implement this function.
        count --;
        Node* temp = root->child;
        Node* oldroot = root;
        Node* temsib;
        std::deque <Node*> multipass;
        root = nullptr;
        while (temp != nullptr){
            temsib = temp->sibling;
            temp->sibling = nullptr;
            multipass.push_back(temp);
            temp = temsib;
        }
        while ( multipass.size() > 1){
            Node* temp1;
            Node* temp2;
            temp1 = multipass.front();
            multipass.pop_front();
            if (multipass.empty()){
                multipass.push_back(temp1);
                break;
            }
            temp2 = multipass.front();
            multipass.pop_front();
            //temp1->parent = temp1->sibling = temp2->parent = temp->sibling = nullptr;
            multipass.push_back(meld(temp1, temp2));
        }
        if (!multipass.empty()){
            Node* rr = multipass.front();
            multipass.pop_back();
            root = rr;
        }
        delete oldroot;
    } // pop()


    // Description: Return the most extreme (defined by 'compare') element of
    //              the pairing heap. This should be a reference for speed.
    //              It MUST be const because we cannot allow it to be
    //              modified, as that might make it no longer be the most
    //              extreme element.
    // Runtime: O(1)
    virtual const TYPE &top() const {
        // TODO: Implement this function

        // These lines are present only so that this provided file compiles.
        return root->elt;
    } // top()


    // Description: Get the number of elements in the pairing heap.
    // Runtime: O(1)
    virtual std::size_t size() const {
        // TODO: Implement this function
        return count; // TODO: Delete or change this line
    } // size()

    // Description: Return true if the pairing heap is empty.
    // Runtime: O(1)
    virtual bool empty() const {
        // TODO: Implement this function
        if (count == 0) return true;
        return false; // TODO: Delete or change this line
    } // empty()


    // Description: Updates the priority of an element already in the pairing
    //              heap by replacing the element refered to by the Node with
    //              new_value. Must maintain pairing heap invariants.
    //
    // PRECONDITION: The new priority, given by 'new_value' must be more
    //               extreme (as defined by comp) than the old priority.
    //
    // Runtime: As discussed in reading material.
    void updateElt(Node* node, const TYPE &new_value) {
        // TODO: Implement this function
        // These lines are present only so that this provided file compiles.
         node->elt = new_value;
         if (node == root ||this->compare(new_value ,node->parent->getElt())) return;
         else {
            Node* leftmost = node->parent->child;
            if (leftmost == node){
                node->parent->child = node->sibling;
                node->sibling = nullptr;
                node->parent = nullptr;
                root = meld(root, node);
            }
            else{
                while (leftmost->sibling != node){
                    leftmost = leftmost->sibling;
                }
                leftmost->sibling = node->sibling;
                node->sibling = nullptr;
                node->parent = nullptr;
                root = meld(root, node);
                }
         }
        
    } // updateElt()


    // Description: Add a new element to the pairing heap. Returns a Node*
    //              corresponding to the newly added element.
    // NOTE: Whenever you create a node, and thus return a Node *, you must
    //       be sure to never move or copy/delete that node in the future,
    //       until it is eliminated by the user calling pop(). Remember this
    //       when you implement updateElt() and updatePriorities().
    // Runtime: O(1)
    Node* addNode(const TYPE &val) {
        // TODO: Implement this function

        // This line is present only so that this provided file compiles.
        Node* n = new Node(val);
        count ++;
        if (root == nullptr) root = n;
        else this->root = meld(n, root);
        return n;
    } // addNode()


private:
    // TODO: Add any additional member variables or member functions you
    //       require here.
    // TODO: We recommend creating a 'meld' function (see the Pairing Heap
    //       papers).

    // NOTE: For member variables, you are only allowed to add a "root
    //       pointer" and a "count" of the number of nodes. Anything else
    //       (such as a deque) should be declared inside of member functions
    //       as needed.
    size_t count;
    Node* root;

    Node* meld(Node* nodeA, Node* nodeB){
        if (nodeA == nullptr) return nodeB;
        else if (nodeB == nullptr) return nodeA;

        if(this->compare(nodeA->getElt(),nodeB->getElt())){
            nodeA->sibling = nodeB->child;
            nodeB->child = nodeA;
            nodeA->parent = nodeB;
            return nodeB;
        }
        else {
            nodeB->parent = nodeA;
            nodeB->sibling = nodeA->child;
            nodeA->child = nodeB;
            return nodeA;
        }
    }
    
};


#endif // PAIRINGPQ_H
