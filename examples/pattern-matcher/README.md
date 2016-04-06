Pattern Matcher Examples
------------------------

See `../guile/README` for an intro on running the guile shell needed
to run the demos here.

The first set of demos provide a basic introduction to various
main features.

* simple.scm: A basic introduction to pattern matching.
* glob.scm: Matching multiple atoms at once.
* choice.scm: Using the ChoiceLink to explore alternatives.
* gsn.scm: Calling arbitrary functions upon a match.
* gpn.scm: Calling arbitrary functions to decide a match.
* sequence.scm: Using GPN's to execute a sequence of tasks.
* condition.scm: combining GPN's and GSN's to make an action taken
    depend on a precondition.
* presence.scm: various ways of testing for the presence of an atom in
    the AtomSpace.
* absent.scm: Use the StateLink to set state; use AbsentLink to check
    for the absence of atoms in the AtomSpace.
* define.scm: using DefineLinks to create patterns out of parts.
* type-signature.scm: using signatures or "deep types" to refine the
    search.

Some simpler applications showing some things one can do, and how to do
them.

* fsm-basic.scm: A simple Deterministic Finite State Machine demo.
* fsm-full.scm: A generic deterministic finite-state-machine constructor.
* fsm-mealy.scm: A generic Mealy machine constructor.
* markov-chain.scm: A Markov chain (probabilistic FSM) based on fsm-full.

Examples that demonstrate some of the inner workings of the pattern
matcher.

* virtual.scm: Using virtual links, and the resulting combinatorial
     explosion.
