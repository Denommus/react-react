type state('a);

let componentFromSignal:
  (
    ~propsEq: ('a, 'a) => bool=?,
    ReasonReact.componentSpec(
      state('a),
      ReasonReact.stateless,
      ReasonReact.noRetainedProps,
      ReasonReact.noRetainedProps,
      ReasonReact.reactElement
    ),
    'a,
    ReactFrp.React.signal('a) => ReactFrp.React.signal(ReasonReact.reactElement)
  ) =>
  ReasonReact.componentSpec(
    state('a),
    state('a),
    ReasonReact.noRetainedProps,
    ReasonReact.noRetainedProps,
    ReasonReact.reactElement
  );

let emitEventToStream: ('a => 'b, ReactEventRe.Form.t) => 'b;

module Utils: {
  open ReactFrp.React;
  let eventFromPromise: Js.Promise.t('a) => event(Js.Result.t('a, Js.Promise.error));
  let eventJoin: event(event('a)) => event('a);
  let eventBind: (event('a), 'a => event('b)) => event('b);
};
