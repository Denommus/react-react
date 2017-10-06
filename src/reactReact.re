open ReactFrp.React;

module type ReactModule = {
  type t 'a;
  let map: eq::('b => 'b => bool)? => ('a => 'b) => t 'a => t 'b;
  let create: eq::(unit => unit => bool)? => unit => (t unit, step::step? => unit => unit);
};

module ComponentGenerator (R: ReactModule) => {
  type action 'a =
    | Tick 'a;
  type state = {
    vdom: ReasonReact.reactElement,
    subscriberId: ref (R.t unit)
  };
  let componentFrom ::eq=(==) name vdomFRP => {
    let component = ReasonReact.reducerComponent name;
    {
      ...component,
      initialState: fun () => {
        vdom: ReasonReact.stringToElement "",
        subscriberId: ref (fst (R.create ()))
      },
      reducer: fun action state =>
        switch action {
        | Tick x => ReasonReact.Update {...state, vdom: x}
        },
      didMount: fun {state, reduce} => {
        state.subscriberId := R.map ::eq (fun x => reduce (fun _event => Tick x) ()) vdomFRP;
        ReasonReact.NoUpdate
      },
      render: fun {state: {vdom}} => vdom
    }
  };
};

module SHelper: ReactModule with type t 'a = signal 'a = {
  include S;
};

module ComponentFromSignal = ComponentGenerator SHelper;

let componentFromSignal ::eq=(==) name vdomS => ComponentFromSignal.componentFrom ::eq name vdomS;

module EHelper: ReactModule with type t 'a = event 'a = {
  include E;
  let map eq::(_: 'b => 'b => bool)=(==) f e => E.map f e;
  let create eq::(_: unit => unit => bool)=(==) () => E.create ();
};

module ComponentFromEvent = ComponentGenerator EHelper;

let componentFromEvent name vdomE => ComponentFromEvent.componentFrom name vdomE;
