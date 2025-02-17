// Copyright 2014 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/service_worker/wait_until_observer.h"

#include "third_party/blink/public/mojom/service_worker/service_worker_event_status.mojom-blink.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/renderer/bindings/core/v8/script_function.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise.h"
#include "third_party/blink/renderer/bindings/core/v8/script_value.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/core/execution_context/agent.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/modules/service_worker/service_worker_global_scope.h"
#include "third_party/blink/renderer/platform/scheduler/public/event_loop.h"
#include "third_party/blink/renderer/platform/scheduler/public/thread.h"
#include "third_party/blink/renderer/platform/web_test_support.h"
#include "v8/include/v8.h"

namespace blink {

namespace {

// Timeout before a service worker that was given window interaction
// permission loses them. The unit is seconds.
const unsigned kWindowInteractionTimeout = 10;
const unsigned kWindowInteractionTimeoutForTest = 1;

base::TimeDelta WindowInteractionTimeout() {
  return base::Seconds(WebTestSupport::IsRunningWebTest()
                           ? kWindowInteractionTimeoutForTest
                           : kWindowInteractionTimeout);
}

}  // anonymous namespace

// According from step 4 of ExtendableEvent::waitUntil() in spec:
// https://w3c.github.io/ServiceWorker/#dom-extendableevent-waituntil
// "Upon fulfillment or rejection of f, queue a microtask to run these
// substeps: Decrement the pending promises count by one."
class WaitUntilObserver::ThenFulfilled final
    : public ThenCallable<IDLUndefined, ThenFulfilled> {
 public:
  explicit ThenFulfilled(WaitUntilObserver* observer) : observer_(observer) {}

  void Trace(Visitor* visitor) const override {
    visitor->Trace(observer_);
    ThenCallable<IDLUndefined, ThenFulfilled>::Trace(visitor);
  }

  void React(ScriptState*) {
    DCHECK(observer_);
    observer_->OnPromiseFulfilled();
    observer_ = nullptr;
  }

 private:
  Member<WaitUntilObserver> observer_;
};

class WaitUntilObserver::ThenRejected final
    : public ThenCallable<IDLAny, ThenRejected, IDLAny> {
 public:
  explicit ThenRejected(WaitUntilObserver* observer) : observer_(observer) {}

  void Trace(Visitor* visitor) const override {
    visitor->Trace(observer_);
    ThenCallable<IDLAny, ThenRejected, IDLAny>::Trace(visitor);
  }

  ScriptValue React(ScriptState* script_state, ScriptValue value) {
    DCHECK(observer_);
    observer_->OnPromiseRejected();
    observer_ = nullptr;
    return ScriptValue(
        script_state->GetIsolate(),
        ScriptPromise<IDLUndefined>::Reject(script_state, value).V8Promise());
  }

 private:
  Member<WaitUntilObserver> observer_;
};

void WaitUntilObserver::WillDispatchEvent() {
  DCHECK(GetExecutionContext());

  // When handling a notificationclick, paymentrequest, or backgroundfetchclick
  // event, we want to allow one window to be focused or opened. These calls are
  // allowed between the call to willDispatchEvent() and the last call to
  // DecrementPendingPromiseCount(). If waitUntil() isn't called, that means
  // between willDispatchEvent() and didDispatchEvent().
  if (type_ == kNotificationClick || type_ == kPaymentRequest ||
      type_ == kBackgroundFetchClick) {
    GetExecutionContext()->AllowWindowInteraction();
  }

  DCHECK_EQ(EventDispatchState::kInitial, event_dispatch_state_);
  event_dispatch_state_ = EventDispatchState::kDispatching;
}

void WaitUntilObserver::DidDispatchEvent(bool event_dispatch_failed) {
  event_dispatch_state_ = event_dispatch_failed
                              ? EventDispatchState::kFailed
                              : EventDispatchState::kDispatched;
  MaybeCompleteEvent();
}

// https://w3c.github.io/ServiceWorker/#dom-extendableevent-waituntil
bool WaitUntilObserver::WaitUntil(
    ScriptState* script_state,
    const ScriptPromise<IDLUndefined>& script_promise,
    ExceptionState& exception_state) {
  DCHECK_NE(event_dispatch_state_, EventDispatchState::kInitial);

  // 1. `If the isTrusted attribute is false, throw an "InvalidStateError"
  // DOMException.`
  // This might not yet be implemented.

  // 2. `If not active, throw an "InvalidStateError" DOMException.`
  if (!IsEventActive()) {
    exception_state.ThrowDOMException(
        DOMExceptionCode::kInvalidStateError,
        "The event handler is already finished and no extend lifetime "
        "promises are outstanding.");
    return false;
  }

  // When handling a notificationclick event, we want to allow one window to
  // be focused or opened. See comments in ::willDispatchEvent(). When
  // waitUntil() is being used, opening or closing a window must happen in a
  // timeframe specified by windowInteractionTimeout(), otherwise the calls
  // will fail.
  if (type_ == kNotificationClick) {
    consume_window_interaction_timer_.StartOneShot(WindowInteractionTimeout(),
                                                   FROM_HERE);
  }

  // 3. `Add f to the extend lifetime promises.`
  // 4. `Increment the pending promises count by one.`
  IncrementPendingPromiseCount();
  script_promise.Then(script_state, MakeGarbageCollected<ThenFulfilled>(this),
                      MakeGarbageCollected<ThenRejected>(this));
  return true;
}

// https://w3c.github.io/ServiceWorker/#extendableevent-active
bool WaitUntilObserver::IsEventActive() const {
  // `An ExtendableEvent object is said to be active when its timed out flag is
  // unset and either its pending promises count is greater than zero or its
  // dispatch flag is set.`
  return pending_promises_ > 0 || IsDispatchingEvent();
}

bool WaitUntilObserver::IsDispatchingEvent() const {
  return event_dispatch_state_ == EventDispatchState::kDispatching;
}

WaitUntilObserver::WaitUntilObserver(ExecutionContext* context,
                                     EventType type,
                                     int event_id)
    : ExecutionContextClient(context),
      type_(type),
      event_id_(event_id),
      consume_window_interaction_timer_(
          context->GetTaskRunner(TaskType::kUserInteraction),
          this,
          &WaitUntilObserver::ConsumeWindowInteraction) {}

void WaitUntilObserver::OnPromiseFulfilled() {
  DecrementPendingPromiseCount();
}

void WaitUntilObserver::OnPromiseRejected() {
  has_rejected_promise_ = true;
  DecrementPendingPromiseCount();
}

void WaitUntilObserver::IncrementPendingPromiseCount() {
  ++pending_promises_;
}

void WaitUntilObserver::DecrementPendingPromiseCount() {
  DCHECK_GT(pending_promises_, 0);
  --pending_promises_;
  MaybeCompleteEvent();
}

void WaitUntilObserver::MaybeCompleteEvent() {
  if (!GetExecutionContext())
    return;

  switch (event_dispatch_state_) {
    case EventDispatchState::kInitial:
      NOTREACHED();
    case EventDispatchState::kDispatching:
      // Still dispatching, do not complete the event.
      return;
    case EventDispatchState::kDispatched:
      // Still waiting for a promise, do not complete the event.
      if (pending_promises_ != 0)
        return;
      // Dispatch finished and there are no pending promises, complete the
      // event.
      break;
    case EventDispatchState::kFailed:
      // Dispatch had some error, complete the event immediately.
      break;
  }

  ServiceWorkerGlobalScope* service_worker_global_scope =
      To<ServiceWorkerGlobalScope>(GetExecutionContext());
  mojom::ServiceWorkerEventStatus status =
      (event_dispatch_state_ == EventDispatchState::kFailed ||
       has_rejected_promise_)
          ? mojom::ServiceWorkerEventStatus::REJECTED
          : mojom::ServiceWorkerEventStatus::COMPLETED;
  switch (type_) {
    case kAbortPayment:
      service_worker_global_scope->DidHandleAbortPaymentEvent(event_id_,
                                                              status);
      break;
    case kActivate:
      service_worker_global_scope->DidHandleActivateEvent(event_id_, status);
      break;
    case kCanMakePayment:
      service_worker_global_scope->DidHandleCanMakePaymentEvent(event_id_,
                                                                status);
      break;
    case kCookieChange:
      service_worker_global_scope->DidHandleCookieChangeEvent(event_id_,
                                                              status);
      break;
    case kFetch:
      service_worker_global_scope->DidHandleFetchEvent(event_id_, status);
      break;
    case kInstall:
      To<ServiceWorkerGlobalScope>(*GetExecutionContext())
          .SetIsInstalling(false);
      service_worker_global_scope->DidHandleInstallEvent(event_id_, status);
      break;
    case kMessage:
      service_worker_global_scope->DidHandleExtendableMessageEvent(event_id_,
                                                                   status);
      break;
    case kMessageerror:
      service_worker_global_scope->DidHandleExtendableMessageEvent(event_id_,
                                                                   status);
      break;
    case kNotificationClick:
      service_worker_global_scope->DidHandleNotificationClickEvent(event_id_,
                                                                   status);
      consume_window_interaction_timer_.Stop();
      ConsumeWindowInteraction(nullptr);
      break;
    case kNotificationClose:
      service_worker_global_scope->DidHandleNotificationCloseEvent(event_id_,
                                                                   status);
      break;
    case kPush:
      service_worker_global_scope->DidHandlePushEvent(event_id_, status);
      break;
    case kPushSubscriptionChange:
      service_worker_global_scope->DidHandlePushSubscriptionChangeEvent(
          event_id_, status);
      break;
    case kSync:
      service_worker_global_scope->DidHandleSyncEvent(event_id_, status);
      break;
    case kPeriodicSync:
      service_worker_global_scope->DidHandlePeriodicSyncEvent(event_id_,
                                                              status);
      break;
    case kPaymentRequest:
      service_worker_global_scope->DidHandlePaymentRequestEvent(event_id_,
                                                                status);
      break;
    case kBackgroundFetchAbort:
      service_worker_global_scope->DidHandleBackgroundFetchAbortEvent(event_id_,
                                                                      status);
      break;
    case kBackgroundFetchClick:
      service_worker_global_scope->DidHandleBackgroundFetchClickEvent(event_id_,
                                                                      status);
      break;
    case kBackgroundFetchFail:
      service_worker_global_scope->DidHandleBackgroundFetchFailEvent(event_id_,
                                                                     status);
      break;
    case kBackgroundFetchSuccess:
      service_worker_global_scope->DidHandleBackgroundFetchSuccessEvent(
          event_id_, status);
      break;
    case kContentDelete:
      service_worker_global_scope->DidHandleContentDeleteEvent(event_id_,
                                                               status);
      break;
  }
}

void WaitUntilObserver::ConsumeWindowInteraction(TimerBase*) {
  if (ExecutionContext* context = GetExecutionContext())
    context->ConsumeWindowInteraction();
}

void WaitUntilObserver::Trace(Visitor* visitor) const {
  visitor->Trace(consume_window_interaction_timer_);
  ExecutionContextClient::Trace(visitor);
}

}  // namespace blink
