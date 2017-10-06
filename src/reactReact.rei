module ComponentFromSignal: {type action 'a; type state;};

module ComponentFromEvent: {type action 'a; type state;};

let componentFromSignal:
  eq::(unit => unit => bool)? =>
  string =>
  ReactFrp.React.signal ReasonReact.reactElement =>
  ReasonReact.componentSpec
    ComponentFromSignal.state
    ComponentFromSignal.state
    ReasonReact.noRetainedProps
    ReasonReact.noRetainedProps
    (ComponentFromSignal.action ReasonReact.reactElement);

let componentFromEvent:
  string =>
  ReactFrp.React.event ReasonReact.reactElement =>
  ReasonReact.componentSpec
    ComponentFromEvent.state
    ComponentFromEvent.state
    ReasonReact.noRetainedProps
    ReasonReact.noRetainedProps
    (ComponentFromEvent.action ReasonReact.reactElement);
