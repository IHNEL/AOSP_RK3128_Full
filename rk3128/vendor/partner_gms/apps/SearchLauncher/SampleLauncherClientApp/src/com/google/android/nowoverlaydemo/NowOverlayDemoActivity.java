package com.google.android.nowoverlaydemo;
/*
**
** Copyright 2016, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

import android.app.Activity;
import android.appwidget.AppWidgetHost;
import android.appwidget.AppWidgetHostView;
import android.appwidget.AppWidgetManager;
import android.appwidget.AppWidgetProviderInfo;
import android.content.ComponentName;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Point;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.CompoundButton;
import android.widget.ToggleButton;
import com.google.android.libraries.launcherclient.LauncherClient;
import com.google.android.libraries.launcherclient.LauncherClientCallbacksAdapter;

/**
 * Now Overlay Demo - Shows how to use the LauncherClient API to attach a Now overlay as -1 screen.
 */
public class NowOverlayDemoActivity extends Activity {

  private LauncherClient mClient;

  private AppWidgetHost mAppWidgetHost;
  private AppWidgetManager mAppWidgetManager;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.main);
    mClient = new LauncherClient(this, new MyDrawerCallbacks(findViewById(R.id.translateView)));

    ((ToggleButton) findViewById(R.id.hotwordToggle))
        .setOnCheckedChangeListener(
            new CompoundButton.OnCheckedChangeListener() {
              @Override
              public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mClient.requestHotwordDetection(isChecked);
              }
            });
    findViewById(R.id.nowDragger).setOnTouchListener(new NowDragger());

    mAppWidgetManager = AppWidgetManager.getInstance(this);
    mAppWidgetHost = new AppWidgetHost(this, R.id.APPWIDGET_HOST_ID);
    addGoogleSearchWidget();
  }

  @Override
  public void onAttachedToWindow() {
    super.onAttachedToWindow();
    mClient.onAttachedToWindow();
  }

  @Override
  public void onDetachedFromWindow() {
    mClient.onDetachedFromWindow();
    super.onDetachedFromWindow();
  }

  @Override
  protected void onPause() {
    mClient.onPause();
    super.onPause();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mClient.onResume();
  }

  @Override
  protected void onDestroy() {
    mClient.onDestroy();
    super.onDestroy();
  }

  public void onOpenOverlayClicked(View v) {
    mClient.showOverlay(true);
  }

  /**
   * Callback to handle view transition as the Now overlay opens/closes
   */
  private static class MyDrawerCallbacks extends LauncherClientCallbacksAdapter {

    private final boolean mIsRtl;
    private final View mTranslateView;

    MyDrawerCallbacks(View v) {
      mTranslateView = v;
      mIsRtl = v.getResources().getConfiguration().getLayoutDirection()
          == View.LAYOUT_DIRECTION_RTL;
    }

    /**
     * Called every time the overlay changes its position.
     * @param progress [0-1] fraction of the screen width covered by Now overlay
     */
    @Override
    public void onOverlayScrollChanged(float progress) {
      mTranslateView.setTranslationX(progress * mTranslateView.getMeasuredWidth()
          * (mIsRtl ? -1 : 1));
    }
  }

  /**
   * To manually handle the Now overlay drag, the following sequence of events should be send
   * LauncherClient#startMove() to start a drag interation
   * LauncherClient#updateMove(progress) multiple calls as the drag progresses
   * LauncherClient#endMove() when the interation ends. At this point, the overlay will complete
   * the transition and open/close itself accordingly.
   */
  private class NowDragger implements View.OnTouchListener {

    private final boolean mIsRtl;
    private float mDownX;

    NowDragger() {
      mIsRtl = getResources().getConfiguration().getLayoutDirection() == View.LAYOUT_DIRECTION_RTL;
    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
      switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN:
          mDownX = event.getX();
          mClient.startMove();
          return true;
        case MotionEvent.ACTION_MOVE: {
          float delta = mIsRtl ? (mDownX - event.getX()) : (event.getX() - mDownX);
          mClient.updateMove(
              Math.max(0, Math.min(1, delta / v.getMeasuredWidth())));
          return true;
        }
        case MotionEvent.ACTION_UP:
        case MotionEvent.ACTION_CANCEL:
          mClient.endMove();
          return true;
      }
      return false;
    }
  }

//============  Widget support code ============ //
 private static final String GOOGLE_SEARCH_WIDGET_CLASSNAME =
     "com.google.android.googlequicksearchbox.SearchWidgetProvider";
 private static final String GOOGLE_SEARCH_PACKAGE_NAME =
     "com.google.android.googlequicksearchbox";
 private static final String GOOGLE_SEARCH_WIDGET_ID_PREFERENCE = "googleSearchId";
 private static final int REQUEST_BIND_APPWIDGET = 4329;

 @Override
 protected void onStart() {
   super.onStart();
   mAppWidgetHost.startListening();
 }

 @Override
 protected void onStop() {
   mAppWidgetHost.stopListening();
   super.onStop();
 }

 private void addGoogleSearchWidget() {
   SharedPreferences prefs = getPreferences(MODE_PRIVATE);
   int appWidgetId = prefs.getInt(GOOGLE_SEARCH_WIDGET_ID_PREFERENCE, -1);

   if (appWidgetId == -1) {
     bindGoogleSearchWidget();
   } else {
     addWidgetToView(appWidgetId, mAppWidgetManager.getAppWidgetInfo(appWidgetId));
   }
 }

 @Override
 protected void onActivityResult(int requestCode, int resultCode, Intent data) {
   if (requestCode != REQUEST_BIND_APPWIDGET || data == null) {
     return;
   }

   int appWidgetId = data.getExtras().getInt(AppWidgetManager.EXTRA_APPWIDGET_ID, -1);
   if (resultCode == Activity.RESULT_OK) {
     AppWidgetProviderInfo appWidgetInfo = mAppWidgetManager.getAppWidgetInfo(appWidgetId);
     storeAppWidgetId(appWidgetId);
     addWidgetToView(appWidgetId, appWidgetInfo);
   } else if (resultCode == Activity.RESULT_CANCELED) {
     mAppWidgetHost.deleteAppWidgetId(appWidgetId);
   }
 }

 private void bindGoogleSearchWidget() {
   int appWidgetId = this.mAppWidgetHost.allocateAppWidgetId();

   ComponentName googleSearch =
       new ComponentName(GOOGLE_SEARCH_PACKAGE_NAME, GOOGLE_SEARCH_WIDGET_CLASSNAME);

   if (!mAppWidgetManager.bindAppWidgetIdIfAllowed(appWidgetId, googleSearch)) {
     // Needs user permission.
     Intent bindIntent = new Intent(AppWidgetManager.ACTION_APPWIDGET_BIND);
     bindIntent.putExtra(AppWidgetManager.EXTRA_APPWIDGET_PROVIDER, googleSearch);
     bindIntent.putExtra(AppWidgetManager.EXTRA_APPWIDGET_ID, appWidgetId);
     startActivityForResult(bindIntent, REQUEST_BIND_APPWIDGET);
   } else {
     storeAppWidgetId(appWidgetId);
   }
 }

 private void storeAppWidgetId(int appWidgetId) {
   SharedPreferences.Editor editor = getPreferences(MODE_PRIVATE).edit();
   editor.putInt(GOOGLE_SEARCH_WIDGET_ID_PREFERENCE, appWidgetId);
   editor.apply();
 }

 private void addWidgetToView(int appWidgetId, AppWidgetProviderInfo info) {
   AppWidgetHostView hostView = mAppWidgetHost.createView(this, appWidgetId, info);
   hostView.setAppWidget(appWidgetId, info);

   Point size = new Point();
   getWindowManager().getDefaultDisplay().getSize(size);
   int minSize = Math.min(size.x, size.y);
   int maxSize = Math.max(size.x, size.y);
   hostView.updateAppWidgetSize(new Bundle(), minSize, minSize, maxSize, maxSize);
   ((ViewGroup) findViewById(R.id.widgetContainer)).addView(hostView);
 }
}
