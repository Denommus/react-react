type action 'a;

type state 'a;

let componentFromSignal:
  propsEq::('a => 'a => bool)? =>
  ReasonReact.componentSpec
    (state 'a)
    ReasonReact.stateless
    ReasonReact.noRetainedProps
    ReasonReact.noRetainedProps
    (action ReasonReact.reactElement) =>
  'a =>
  (ReactFrp.React.signal 'a => ReactFrp.React.signal ReasonReact.reactElement) =>
  ReasonReact.componentSpec
    (state 'a)
    (state 'a)
    ReasonReact.noRetainedProps
    ReasonReact.noRetainedProps
    (action ReasonReact.reactElement);

let emitEventToStream: ('a => 'b) => ReactEventRe.Form.t => 'b;
