// Copyright (c) 2013 GitHub, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_NATIVE_WINDOW_H_
#define ATOM_BROWSER_NATIVE_WINDOW_H_

#include <iosfwd>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "browser/native_window_observer.h"
#include "content/public/browser/notification_registrar.h"
#include "content/public/browser/notification_observer.h"
#include "content/public/browser/web_contents_delegate.h"
#include "content/public/browser/web_contents_observer.h"

namespace base {
class DictionaryValue;
class ListValue;
}

namespace brightray {
class InspectableWebContents;
}

namespace content {
class BrowserContext;
class WebContents;
}

namespace gfx {
class Point;
class Rect;
class Size;
}

namespace atom {

class AtomJavaScriptDialogManager;

class NativeWindow : public content::WebContentsDelegate,
                     public content::WebContentsObserver,
                     public content::NotificationObserver {
 public:
  virtual ~NativeWindow();

  // Create window with existing WebContents.
  static NativeWindow* Create(content::WebContents* web_contents,
                              base::DictionaryValue* options);

  // Create window with new WebContents.
  static NativeWindow* Create(base::DictionaryValue* options);

  // Find a window from its process id and routing id.
  static NativeWindow* FromRenderView(int process_id, int routing_id);

  void InitFromOptions(base::DictionaryValue* options);

  virtual void Close() = 0;
  virtual void CloseImmediately() = 0;
  virtual void Move(const gfx::Rect& pos) = 0;
  virtual void Focus(bool focus) = 0;
  virtual void Show() = 0;
  virtual void Hide() = 0;
  virtual void Maximize() = 0;
  virtual void Unmaximize() = 0;
  virtual void Minimize() = 0;
  virtual void Restore() = 0;
  virtual void SetFullscreen(bool fullscreen) = 0;
  virtual bool IsFullscreen() = 0;
  virtual void SetSize(const gfx::Size& size) = 0;
  virtual gfx::Size GetSize() = 0;
  virtual void SetMinimumSize(const gfx::Size& size) = 0;
  virtual gfx::Size GetMinimumSize() = 0;
  virtual void SetMaximumSize(const gfx::Size& size) = 0;
  virtual gfx::Size GetMaximumSize() = 0;
  virtual void SetResizable(bool resizable) = 0;
  virtual bool IsResizable() = 0;
  virtual void SetAlwaysOnTop(bool top) = 0;
  virtual bool IsAlwaysOnTop() = 0;
  virtual void SetPosition(const std::string& position) = 0;
  virtual void SetPosition(const gfx::Point& position) = 0;
  virtual gfx::Point GetPosition() = 0;
  virtual void SetTitle(const std::string& title) = 0;
  virtual std::string GetTitle() = 0;
  virtual void FlashFrame(bool flash) = 0;
  virtual void SetKiosk(bool kiosk) = 0;
  virtual bool IsKiosk() = 0;
  virtual gfx::NativeWindow GetNativeWindow() = 0;

  virtual bool IsClosed() const { return is_closed_; }
  virtual void ShowDevTools();
  virtual void CloseDevTools();

  // The same with closing a tab in a real browser.
  //
  // Should be called by platform code when user want to close the window.
  virtual void CloseWebContents();

  content::WebContents* GetWebContents() const;

  void AddObserver(NativeWindowObserver* obs) {
    observers_.AddObserver(obs);
  }

  void RemoveObserver(NativeWindowObserver* obs) {
    observers_.RemoveObserver(obs);
  }

 protected:
  explicit NativeWindow(content::WebContents* web_contents,
                        base::DictionaryValue* options);

  brightray::InspectableWebContents* inspectable_web_contents() const {
    return inspectable_web_contents_.get();
  }

  void NotifyWindowClosed();

  // Implementations of content::WebContentsDelegate.
  virtual void WebContentsCreated(content::WebContents* source_contents,
                                  int64 source_frame_id,
                                  const string16& frame_name,
                                  const GURL& target_url,
                                  content::WebContents* new_contents) OVERRIDE;
  virtual content::JavaScriptDialogManager*
      GetJavaScriptDialogManager() OVERRIDE;
  virtual void BeforeUnloadFired(content::WebContents* tab,
                                 bool proceed,
                                 bool* proceed_to_fire_unload) OVERRIDE;
  virtual void CloseContents(content::WebContents* source) OVERRIDE;

  // Implementations of content::WebContentsObserver.
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;

  // Implementations of content::NotificationObserver
  virtual void Observe(int type,
                       const content::NotificationSource& source,
                       const content::NotificationDetails& details) OVERRIDE;

 private:
  void OnRendererMessage(const std::string& channel,
                         const base::ListValue& args);

  void OnRendererMessageSync(const std::string& channel,
                             const base::ListValue& args,
                             base::DictionaryValue* result);

  // Notification manager.
  content::NotificationRegistrar registrar_;

  // Observers of this window.
  ObserverList<NativeWindowObserver> observers_;

  bool is_closed_;

  scoped_ptr<AtomJavaScriptDialogManager> dialog_manager_;
  scoped_ptr<brightray::InspectableWebContents> inspectable_web_contents_;

  DISALLOW_COPY_AND_ASSIGN(NativeWindow);
};

}  // namespace atom

#endif  // ATOM_BROWSER_NATIVE_WINDOW_H_
