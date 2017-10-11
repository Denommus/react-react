open ReactFrp.React;

type action 'a =
  | Tick 'a;

type state 'a = {
  propsF: ref (option (step::step? => 'a => unit)),
  subscription: ref (option (signal unit)),
  vdom: ReasonReact.reactElement
};

let componentFromSignal
    ::propsEq=(==)
    (
      component:
        ReasonReact.componentSpec
          (state 'a)
          ReasonReact.stateless
          ReasonReact.noRetainedProps
          ReasonReact.noRetainedProps
          (action ReasonReact.reactElement)
    )
    props
    propsToVdom => {
  ...component,
  initialState: fun () => {
    propsF: ref None,
    subscription: ref None,
    vdom: ReasonReact.stringToElement ""
  },
  reducer: fun action state =>
    switch action {
    | Tick x => ReasonReact.Update {...state, vdom: x}
    },
  willReceiveProps: fun self =>
    switch !self.state.propsF {
    | Some f =>
      f props;
      self.state
    | None => self.state
    },
  didMount: fun {state, reduce} => {
    let (propsS, propsF) = S.create eq::propsEq props;
    state.propsF := Some propsF;
    let vdomS = propsToVdom propsS;
    state.subscription :=
      Some (S.map (fun newVdom => reduce (fun _event => Tick newVdom) ()) vdomS);
    ReasonReact.NoUpdate
  },
  willUnmount: fun {state} => {
    state.subscription := None;
    ()
  },
  render: fun {state} => state.vdom
};

let valueFromEvent ev => (ev |> ReactEventRe.Form.target |> ReactDOMRe.domElementToObj)##value;

let emitEventToStream signalF ev => valueFromEvent ev |> signalF;
