package com.example.all;

import android.os.Bundle;
import android.view.ViewGroup;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.webkit.WebViewAssetLoader;
import androidx.webkit.WebViewClientCompat;
import androidx.webkit.WebResourceErrorCompat;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        WebView webView = new WebView(this);
        webView.setLayoutParams(new ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
        ));
        setContentView(webView);

        WebSettings settings = webView.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);

        WebViewAssetLoader assetLoader = new WebViewAssetLoader.Builder()
                .addPathHandler("/assets/", new WebViewAssetLoader.AssetsPathHandler(this))
                .build();

        webView.setWebViewClient(new WebViewClientCompat() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                return assetLoader.shouldInterceptRequest(request.getUrl());
            }

            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceErrorCompat error) {
                if (request.isForMainFrame()) {
                    showFallback(view, "WebView load error: " + error.getDescription());
                }
            }
        });

        if (hasBundledWebApp()) {
            webView.loadUrl("https://appassets.androidplatform.net/assets/web/index.html");
        } else {
            showFallback(
                    webView,
                    "Web app not bundled. Please run npm install and npm run build:android in the app directory, then rerun this app."
            );
        }
    }

    private boolean hasBundledWebApp() {
        try {
            getAssets().open("web/index.html").close();
            return true;
        } catch (IOException e) {
            return false;
        }
    }

    private void showFallback(WebView webView, String message) {
        webView.loadDataWithBaseURL(
                null,
                "<html><body style='font-family:sans-serif;padding:24px;'>" +
                        "<h2>Unable to render web app</h2>" +
                        "<p>" + escapeHtml(message) + "</p>" +
                        "</body></html>",
                "text/html",
                "UTF-8",
                null
        );
    }

    private String escapeHtml(String text) {
        return text
                .replace("&", "&amp;")
                .replace("<", "&lt;")
                .replace(">", "&gt;");
    }
}
