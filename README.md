
# ReactReact


## What it is

In JavaScript-land, it's possible to program components in a FRP
style, by using the popular libraries [RxJS](https://github.com/Reactive-Extensions/RxJS) and ReactJS. The bridge
between the two is usually the library [recompose](https://github.com/acdlite/recompose), which provides a
function `componentFromStream`. Since this style allows a great
level of composability, I wanted it on ReasonML as well.

OCaml (which, as you may know, is completely compatible with
ReasonML) also has a FRP library, which also happens to be called
[React](http://erratique.ch/software/react). It has no relationship with ReactJS, it's much more similar
to RxJS.

This library provides the function `componentFromSignal`, which is
like recompose's `componentFromStream`. With this library, you
have a bridge between React and ReasonReact.


## Installation

You need to have a [BuckleScript project](https://bucklescript.github.io/bucklescript/Manual.html#_get_started) already setup.

If you use npm, in your project's directory:

    npm install react-react

Or if you use yarn:

    yarn add @denommus/react-react

After that, edit `bsconfig.json` and include "react-frp",
"react-react", and "reason-react" to the "bs-dependencies"
array.


## How to use it

Going through React's [documentation](http://erratique.ch/software/react/doc/React) is recommended past this point.
Since this library is so simple, most of the features are actually
from React. All the library does is to produce a valid ReactJS
component out of React's signals and events (at least for now, more
features are being planned).

The heart of the library is the `componentFromSignal` function,
which takes:

1.  a function from signal of props to signal of `reactElement`
2.  props (it can be `unit`, or a single prop, or a tuple of props)

So, for instance, you could create a time counter like the
following:

    open ReactFrp.React;
    
    let initial = ref(0);
    
    /* timeS is a signal that represents the current value, timeF is the
       function that changes the signal's value */
    let (timeS, timeF) = S.create(!initial);
    
    /* Increases the counter and updates the signal with the new value */
    let timeIncrement = () => {
      initial := !initial + 1;
      timeF(!initial)
    };
    
    /* Calls timeIncrement every second */
    let timerId = Js.Global.setInterval(timeIncrement, 1000);
    
    /* This is the actual signal function that will produce the component.
       Notice how it's produced by mapping the timeS signal to a component
       signal. I ignore the argument because this specific component
       doesn't have props */
    let vdomS = (_) =>
      S.map(
        ~eq=((_, _) => false),
        (
          (time) => {
            let timeMessage = time == 1 ? "second" : "seconds";
            let greeting = {j|You've spent $time $timeMessage on this page!|j};
            <div> (ReasonReact.string(greeting)) </div>
          }
        ),
        timeS
    );
    
    /* componentFromSignal will apply the signal into a real ReactJS
       component! The unit is passed because this component doesn't have
       props */
    [@react.component]
    let make = () => ReactReact.componentFromSignal(vdomS, ());

Notice that I pass a function `eq` that always returns false to
`S.map`. I do that because equality between
`ReasonReact.reactElement` is always true, and React only updates
the signal when it detects that it has changed. So I need to force
it to understand that it has changed (see [Equality](http://erratique.ch/software/react/doc/React.html#sigeq) in React's
documentation).


### Passing props

To use props, just pass the value of the props as a tuple to
`componentFromSignal`

    open ReactFrp.React;
    
    /* We'll have a component to show the user's input */
    module ShowName = {
      /* Now I won't ignore the first argument */
      let vdomS = (propS) =>
        S.map(
          ~eq=(_, _) => false,
          (name) =>
            if (name != "") {
              let greeting = {j|Hello, $name!|j};
              <p> (ReasonReact.string(greeting)) </p>
            } else {
              <p> (ReasonReact.string("Hello, unknown person!")) </p>
            },
          propsS
        );
      /* Notice how I pass the name prop to componentFromSignal */
      [@react.component]
      let make = (~name) => ReasonReact.componentFromSignal(vdomS, name);
    };
    
    /* This time we won't update the signal automatically, instead we'll
       use user input */
    let (nameS, nameF) = S.create("");
    
    let vdomS =
      S.map(
        ~eq=(_, _) => false,
        (name) =>
          /* emitEventToStream will send the text value of an event to
             the function passed as argument */
          <div>
            <input type_="text" onChange=(ReactReact.Utils.emitEventToStream(nameF)) />
            <ShowName name />
          </div>,
        nameS
      );
    
    [@react.component]
    let make = () => ReactReact.componentFromSignal(vdomS, ());


### Working with side-effects

Impurity should work, but I recommend you to be as pure as
possible with your signals. If you want to debug a signal or
anything similar, you can always subscribe to it with a
`signal(unit)`, like the following

    let subscriber = S.map((vdom) => Js.log(vdom), vdomS);


### Using the result of promises

Promises are, for every purpose, less powerful events, and you can
convert promises to events. If you're using a library that returns
promises (like [bs-fetch](https://github.com/reasonml-community/bs-fetch)), you can call `Utils.eventFromPromise` on
the result. Then you can use it as an `event(Js.Result.t ('a,
    Js.Promise.error))`. This allows you to compose signals out of
promises, and eventually even create components out of these
signals.

It uses `Belt.Result.t` because every promise might fail and throw
an error instead. ReactReact catches that error and returns it in
the result type instead.


### Other util functions

Events in React are actually monads (I know, I know, it's a dirty
word), but the library doesn't directly expose functions to use
them as such (though you may create them youself from the
available functions).

Since that's such a common use-case, ReactReact provides
`Utils.Event.join` and `Utils.Event.bind` so you can do that.

For people who still don't know what monads are: `join` allows you
to take an `event(event('a))` and produce an `event('a)`.

`bind` is similar to Promise's `then_`. It allows you to compose
events


## Running the examples

The [example file](src/example.re) has both examples working together. You can read
the source to see the library in action.

To run it, run in a terminal

    npm install
    npm run start

And in a separate terminal window

    npm run webpack

Then just open the [index.html](src/index.html) file in your browser of choice


## Wishlist


### A functor interface to generate the components

Since the components are quite similar to each other, as can be
seen in the example file, it would be nice if the user could do
something like:

    module FooComponent =
      CreateComponent(
        {
          type props = {name: string};
          let vdomS = (propS) =>
            S.map(
              ~eq=(_, _) => false,
              ({name}) =>
                if (name != "") {
                  let greeting = {j|Hello, $name!|j};
                  <p> (ReasonReact.string(greeting)) </p>
                } else {
                  <p> (ReasonReact.string("Hello, unknown person!")) </p>
                },
              propsS
            );
        }
      );

And then the component would be used like

    <FooComponent name="bar" />

That seems a bit more readable, right? Sadly, it can't be done as
of now because the `make` function takes props as named
parameters, so I can't really generalize it with a functor.


## Disclaimer

I don't actually like using the term "FRP" for things like React or
RxJS, because FRP has an actual [denotational semantics](https://begriffs.com/posts/2015-07-22-essence-of-frp.html) that these
libraries don't respect.

In my opinion, it's not a huge problem, and they're still great
libraries that allow you to make great systems (that's why I'm
supporting React, after all), I just wanted to make it clear that a
more appropriated terminology is needed.

But the terminology got popular, so it's more intuitive for most
people if I just describe this library as a FRP bridge for
ReasonReact.

