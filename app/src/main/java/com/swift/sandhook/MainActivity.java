package com.swift.sandhook;

import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.swift.sandhook.test.Inter;
import com.swift.sandhook.test.InterImpl;
import com.swift.sandhook.test.PendingHookTest;
import com.swift.sandhook.test.TestClass;

import java.lang.reflect.Field;

import de.robv.android.xposed.XposedHelpers;

public class MainActivity extends AppCompatActivity {

    Inter inter;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        methodBeHooked(hashCode(), 1);


        toolbar.postDelayed(new Runnable() {
            @Override
            public void run() {
                methodBeHooked(hashCode(), 3);
                Log.w("gy", "dadadad");
            }
        }, 3000);

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);

        final TestClass str = new TestClass(1);

        str.add1();
        str.add2();
        str.testNewHookApi(this, 1);

        str.jni_test();

        Log.e("dd", str.a + "");

        inter = new InterImpl();

        new Thread(new Runnable() {
            @Override
            public void run() {
                inter.dosth();
                inter = new Inter() {
                    @Override
                    public void dosth() {
                        Log.e("dosth", hashCode() + "");
                    }
                };
                Log.e("testStub", "res = " + testStub(str, 1, "origin b", false, 'x', "origin e"));
            }
        }).start();

        inter.dosth();

        testPluginHook(str);

        MyApp.initedTest = true;
        try {
            PendingHookTest.test();
        } catch (Throwable e) {

        }
    }

    public static Field getField(Class topClass, String fieldName) throws NoSuchFieldException {
        while (topClass != null && topClass != Object.class) {
            try {
                return topClass.getDeclaredField(fieldName);
            } catch (Exception e) {
            }
            topClass = topClass.getSuperclass();
        }
        throw new NoSuchFieldException(fieldName);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    protected void onPause() {
        super.onPause();
        inter = new Inter() {
            @Override
            public void dosth() {
                Log.e("dosth", hashCode() + "");
            }
        };
    }

    public static int methodBeHooked(int a, int b) {
        a = a + 1 + 2;
        b = b + a + 3;
        Log.e("MainActivity", "call methodBeHooked origin");
        return a + b;
    }

    public int testPluginHook(TestClass testClass) {
        Log.e("MainActivity", "call testPluginHook origin");
        return testClass.a;
    }

    public Integer testStub(TestClass testClass, int a, String b, boolean c, char d, String e) {
        Log.e("MainActivity", "call testStub origin" + a + ("" + c) + d + e);
        return a;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}

