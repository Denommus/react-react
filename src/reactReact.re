open ReactFrp.React;

type state('a) = {
  propsF: (~step: step=?, 'a) => unit,
  propsS: signal('a),
  subscription: ref(option(signal(unit))),
  vdom: ReasonReact.reactElement
};

let componentFromSignal =
    (
      ~propsEq=(==),
      component:
        ReasonReact.componentSpec(
          state('a),
          ReasonReact.stateless,
          ReasonReact.noRetainedProps,
          ReasonReact.noRetainedProps,
          ReasonReact.reactElement
        ),
      props,
      propsToVdom
    ) => {
  ...component,
  initialState: () => {
    let (propsS, propsF) = S.create(~eq=propsEq, props);
    {propsF, propsS, subscription: ref(None), vdom: ReasonReact.arrayToElement([||])}
  },
  reducer: (vdom, state) => ReasonReact.Update({...state, vdom}),
  willUpdate: ({newSelf}) => newSelf.state.propsF(props),
  didMount: ({state, reduce}) => {
    let vdomS = propsToVdom(state.propsS);
    let reduceState = (newVdom, _event) => newVdom;
    state.subscription := Some(S.map((newVdom) => reduce(reduceState(newVdom), ()), vdomS));
    ReasonReact.NoUpdate
  },
  willUnmount: ({state}) => {
    state.subscription := None;
    ()
  },
  render: ({state}) => state.vdom
};

module Utils = {
  let valueFromEvent = (ev) => (ev |> ReactEventRe.Form.target |> ReactDOMRe.domElementToObj)##value;
  let emitEventToStream = (signalF, ev) => valueFromEvent(ev) |> signalF;
  let eventFromPromise = (promise) => {
    open Js.Result;
    open Js.Promise;
    let (promiseE, promiseF) = E.create();
    promise
    |> then_(
         (x) => {
           promiseF(Ok(x));
           promise
         }
       )
    |> catch(
         (x) => {
           promiseF(Error(x));
           promise
         }
       )
    |> ignore;
    promiseE
  };
  module Event = {
    let join = (ee) => E.switch_(E.never, ee);
    let bind = (e, f) => join(E.map(f, e));
  };
};
