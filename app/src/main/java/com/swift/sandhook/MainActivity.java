package com.swift.sandhook;

import android.os.Bundle;
import android.widget.TextView;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;

import com.swift.sandhook.wrapper.HookErrorException;
import com.swift.sandhook.wrapper.HookWrapper;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
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

        try {
            HookWrapper.addHookClass(ActivityHooker.class);
        } catch (HookErrorException e) {
            e.printStackTrace();
        }

        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        try {
            Method method1 = ArtMethodSizeTest.class.getDeclaredMethod("method1");
            Method method2 = ArtMethodSizeTest.class.getDeclaredMethod("method2");
            Field field = Class.class.getDeclaredField("dexCache");
            field.setAccessible(true);
            Object dexCache = field.get(ArtMethodSizeTest.class);
            Field resolvedMethodsField = dexCache.getClass().getDeclaredField("resolvedMethods");
            resolvedMethodsField.setAccessible(true);
//            long[] methods = (long[]) resolvedMethodsField.get(dexCache);
//
//
//            Field dexMethodIndexField = getField(Method.class, "dexMethodIndex");
//            dexMethodIndexField.setAccessible(true);
//            int dexMethodIndex = (int) dexMethodIndexField.get(method1);
//
//            Field artMethodField = getField(Method.class, "artMethod");
//            artMethodField.setAccessible(true);
//            long artMethod = (long) artMethodField.get(method1);
//
//            methods[dexMethodIndex] = artMethod;

            SandHook.init();
            initHook();

            tv.setText("" + calArtSize(method1, method2));
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
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

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI(Method method);

    public native int calArtSize(Method method1, Method method2);

    public native void initHook();
}
